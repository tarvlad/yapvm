//!!!!!!!!!!!!!!!!!!!!!!!!!!!! README !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This is very straightforward stupid
// implementation of interpreter with c-style dispatch
// and othed unoptimized things.
// It WILL be rewritten
//
// Any questions please send to tarvlad@inbox.ru
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "interpreter.h"
#include <chrono>
#include <cmath>

#include "logger.h"

static std::atomic_size_t GLOBAL_BORN_THREAD_ID = 71;

// TODO rewrite as Scope method
#define LAST_EXEC_RES_YOBJ static_cast<ManagedObject *>(scope_->get(Scope::lst_exec_res).value().value_)->value()
#define LAST_EXEC_RES_M_YOBJ static_cast<ManagedObject *>(scope_->get(Scope::lst_exec_res).value().value_)

#define LAST_EXEC_RES_YOBJ static_cast<ManagedObject *>(scope_->get(Scope::lst_exec_res).value().value_)->value()

//TODO use condvars for wait


void yapvm::interpreter::Interpreter::handle_safepoint() {
    if (need_park_.load()) {
        parked_.store(true);
        need_park_.store(false);
        while (!need_unpark_.load()) {
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        parked_.store(false);
        need_unpark_.store(false);
    }
}


// used for thread creation
static
std::vector<yapvm::scoped_ptr<Stmt>> copy_vec_no_owning(const std::vector<yapvm::scoped_ptr<Stmt>> &v) {
    std::vector<yapvm::scoped_ptr<Stmt>> ret;
    for (const yapvm::scoped_ptr<Stmt> &p : v) {
        ret.emplace_back(p.get(), false);
    }
    return ret;
}


// SHOULD be called only once when interpreter starts
void yapvm::interpreter::Interpreter::__worker_exec(Module *code) {
    //Logger::log("starting interpreter");

    for (const scoped_ptr<Stmt> &i: code->body()) {
        if (!interpret(i)) {
            break;
        }
        if (finishing_.load()) {
            break;
        }
    }
    while (!thread_manager_->unregister_interpreter(this)) {
        handle_safepoint();
    }
}


// TODO all variables accesses should be uprising lookups
void yapvm::interpreter::Interpreter::interpret_expr(Expr *code) {
    //TODO after every expr exec change lst_expr_res
    if (instanceof<BoolOp>(code)) {
        BoolOp *bool_op = dynamic_cast<BoolOp *>(code);
        interpret_expr(bool_op->values()[0]);

        YObject *first_call_res = LAST_EXEC_RES_YOBJ;
        if (first_call_res->get_typename() != "bool") {
            throw std::runtime_error("Interpreter: BoolOp args should be bools in end of evaluation");
        }
        bool result = first_call_res->get_value_as_bool();

        for (size_t i = 1; i < bool_op->values().size(); i++) {
            interpret_expr(bool_op->values()[i]);
            YObject *call_res = LAST_EXEC_RES_YOBJ;
            if (call_res->get_typename() != "bool") {
                throw std::runtime_error("Interpreter: BoolOp args should be bools in end of evaluation");
            }
            if (dynamic_cast<And *>(bool_op->op().get()) != nullptr) {
                result = result && call_res->get_value_as_bool();
            } else {
                result = result || call_res->get_value_as_bool();
            }
        }

        ManagedObject *resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } }};
        register_queue_.push(resobj);
        scope_->update_last_exec_res(resobj);
        return;
    }
    if (instanceof<BinOp>(code)) {
        BinOp *bin_op = dynamic_cast<BinOp *>(code);
        interpret_expr(bin_op->left());
        // we have guarantees that GC will not happen here because it can happen only after single statement execution
        YObject *left = LAST_EXEC_RES_YOBJ;
        interpret_expr(bin_op->right());
        YObject *right = LAST_EXEC_RES_YOBJ;

        if (left->get_typename() != right->get_typename() && left->get_typename() != "string") {
            throw std::runtime_error("Interpreter: BinOp operands currently need to be same type");
        }

        BinOpKind *op_kind = bin_op->op();
        ManagedObject *resobj = nullptr;
        if (instanceof<Add>(op_kind)) {
            if (left->get_typename() == "bool") {
                ssize_t res = 0;
                if (left->get_value_as_bool()) {
                    res++;
                }
                if (right->get_value_as_bool()) {
                    res++;
                }
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res += right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "float") {
                double res = 0;
                res += left->get_value_as_float();
                res += right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "float", new double{ res } } };
            } else if (left->get_typename() == "string") {
                std::string res;
                res += left->get_value_as_string();
                res += right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "string", new std::string{ std::move(res) } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: Add not supported for " + left->get_typename());
            }
        } else if (instanceof<Sub>(op_kind)) {
            if (left->get_typename() == "bool") {
                ssize_t res = 0;
                if (left->get_value_as_bool()) {
                    res++;
                }
                if (right->get_value_as_bool()) {
                    res--;
                }
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res -= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "float") {
                double res = 0;
                res += left->get_value_as_float();
                res -= right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "float", new double{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: Sub not supported for " + left->get_typename());
            }
        } else if (instanceof<Mult>(op_kind)) {
            if (left->get_typename() == "bool") {
                ssize_t res = 0;
                if (left->get_value_as_bool() && right->get_value_as_bool()) {
                    res = 1;
                }
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res *= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "float") {
                double res = 0;
                res += left->get_value_as_float();
                res *= right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "float", new double{ res } } };
            } else if (left->get_typename() == "string") {
                std::string res;
                if (right->get_typename() != "int") {
                    throw std::runtime_error("Interpreter: Mult for string require int as right argument");
                }
                std::string base = left->get_value_as_string();
                ssize_t times = right->get_value_as_int();
                for (size_t i = 0; i < times; i++) {
                    res += base;
                }
                resobj = new ManagedObject{ new YObject{ "string", new std::string{ std::move(res) } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: Mult not supported for " + left->get_typename());
            }
        } else if (instanceof<Div>(op_kind)) {
            if (left->get_typename() == "int") {
                double res = 0;
                res += static_cast<double>(left->get_value_as_int());
                res /= static_cast<double>(right->get_value_as_int());
                resobj = new ManagedObject{ new YObject{ "float", new double{ res } } };
            } else if (left->get_typename() == "float") {
                double res = 0;
                res += left->get_value_as_float();
                res /= right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "float", new double{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: Div not supported for " + left->get_typename());
            }
        } else if (instanceof<Mod>(op_kind)) {
            if (left->get_typename() == "bool") {
                ssize_t res = 0;
                if (left->get_value_as_bool() && right->get_value_as_bool()) {
                    res = 1;
                }
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res %= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: Mod not supported for " + left->get_typename());
            }
        } else if (instanceof<Pow>(op_kind)) {
            if (left->get_typename() == "bool") {
                ssize_t res = 0;
                if (left->get_value_as_bool() && right->get_value_as_bool()) {
                    res = 1;
                }
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res = static_cast<ssize_t>(std::pow(res, right->get_value_as_int()));
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else if (left->get_typename() == "float") {
                double res = 0;
                res += left->get_value_as_float();
                res = std::pow(res, right->get_value_as_float());
                resobj = new ManagedObject{ new YObject{ "float", new double{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: Pow not supported for " + left->get_typename());
            }
        } else if (instanceof<LShift>(op_kind)) {
            if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res <<= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: LShift not supported for " + left->get_typename());
            }
        } else if (instanceof<RShift>(op_kind)) {
            if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res >>= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: RShift not supported for " + left->get_typename());
            }
        } else if (instanceof<BitOr>(op_kind)) {
            if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res |= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: BitOr not supported for " + left->get_typename());
            }
        } else if (instanceof<BitXor>(op_kind)) {
            if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res ^= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: BitXor not supported for " + left->get_typename());
            }
        } else if (instanceof<BitAnd>(op_kind)) {
            if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res &= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: BitAnd not supported for " + left->get_typename());
            }
        } else if (instanceof<FloorDiv>(op_kind)) {
            if (left->get_typename() == "int") {
                ssize_t res = 0;
                res += left->get_value_as_int();
                res /= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ res } } };
            } else { //TODO
                throw std::runtime_error("Interpreter: BitAnd not supported for " + left->get_typename());
            }
        }
        if (resobj == nullptr) {
            throw std::runtime_error("Interpreter: unexpected BinaryOperatorKind");
        }
        register_queue_.push(resobj);
        scope_->update_last_exec_res(resobj);
        return;
    }
    if (instanceof<UnaryOp>(code)) {
        UnaryOp *unary_op = dynamic_cast<UnaryOp *>(code);
        interpret_expr(unary_op->operand());
        YObject *operand = LAST_EXEC_RES_YOBJ;

        UnaryOpKind *op_kind = unary_op->op();
        ManagedObject *resobj = nullptr;
        if (instanceof<Not>(op_kind)) {
            if (operand->get_typename() == "bool") {
                bool value = !operand->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ value } } };
            } else { // TODO
                throw std::runtime_error("Interpreter: Not not supported for " + operand->get_typename());
            }
        } else if (instanceof<USub>(op_kind)) {
            if (operand->get_typename() == "int") {
                ssize_t value = -operand->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ value } } };
            } else if (operand->get_typename() == "float") {
                double value = -operand->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "float", new double{ value } } };
            } else { // TODO
                throw std::runtime_error("Interpreter: USub not supported for " + operand->get_typename());
            }
        }

        if (resobj == nullptr) {
            throw std::runtime_error("Interpreter: unexpected UnaryOpKind");
        }
        register_queue_.push(resobj);
        scope_->update_last_exec_res(resobj);
        return;
    }
    if (instanceof<Compare>(code)) {
        Compare *compare = dynamic_cast<Compare *>(code);
        if (compare->comparators().size() != 1) {
            throw std::runtime_error("Interpreter: Compare currently supported only with one argument");
        }
        interpret_expr(compare->left());
        YObject *left = LAST_EXEC_RES_YOBJ;
        CmpOpKind *op = compare->ops()[0];
        interpret_expr(compare->comparators()[0]);
        YObject *right = LAST_EXEC_RES_YOBJ;

        if (left->get_typename() != right->get_typename()) {
            ManagedObject *resobj = new ManagedObject{ new YObject{ "bool", new bool{ false } } };
            scope_->update_last_exec_res(resobj);
            return;
        }

        // TODO special handle for lists, dicts???
        ManagedObject *resobj = nullptr;
        if (instanceof<Eq>(op)) {
            if (left->get_typename() == "bool") {
                bool result = left->get_value_as_bool() == right->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "int") {
                bool result = left->get_value_as_int() == right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "float") {
                bool result = left->get_value_as_float() == right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "string") {
                bool result = left->get_value_as_string() == right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else {
                bool result = left == right;
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            }
        } else if (instanceof<NotEq>(op)) {
            if (left->get_typename() == "bool") {
                bool result = left->get_value_as_bool() != right->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "int") {
                bool result = left->get_value_as_int() != right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "float") {
                bool result = left->get_value_as_float() != right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "string") {
                bool result = left->get_value_as_string() != right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else {
                bool result = left != right;
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            }
        } else if (instanceof<Lt>(op)) {
            if (left->get_typename() == "bool") {
                bool result = left->get_value_as_bool() < right->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "int") {
                bool result = left->get_value_as_int() < right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "float") {
                bool result = left->get_value_as_float() < right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "string") {
                bool result = left->get_value_as_string() < right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else { // TODO
                throw std::runtime_error("Interpreter: Lt not supported for " + left->get_typename());
            }
        } else if (instanceof<LtE>(op)) {
            if (left->get_typename() == "bool") {
                bool result = left->get_value_as_bool() <= right->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "int") {
                bool result = left->get_value_as_int() <= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "float") {
                bool result = left->get_value_as_float() <= right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "string") {
                bool result = left->get_value_as_string() <= right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else { // TODO
                throw std::runtime_error("Interpreter: LtE not supported for " + left->get_typename());
            }
        } else if (instanceof<Gt>(op)) {
            if (left->get_typename() == "bool") {
                bool result = left->get_value_as_bool() > right->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "int") {
                bool result = left->get_value_as_int() > right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "float") {
                bool result = left->get_value_as_float() > right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "string") {
                bool result = left->get_value_as_string() > right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else { // TODO
                throw std::runtime_error("Interpreter: Gt not supported for " + left->get_typename());
            }
        } else if (instanceof<GtE>(op)) {
            if (left->get_typename() == "bool") {
                bool result = left->get_value_as_bool() >= right->get_value_as_bool();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "int") {
                bool result = left->get_value_as_int() >= right->get_value_as_int();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "float") {
                bool result = left->get_value_as_float() >= right->get_value_as_float();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else if (left->get_typename() == "string") {
                bool result = left->get_value_as_string() >= right->get_value_as_string();
                resobj = new ManagedObject{ new YObject{ "bool", new bool{ result } } };
            } else { // TODO
                throw std::runtime_error("Interpreter: Gt not supported for " + left->get_typename());
            }
        }
        if (resobj == nullptr) {
            throw std::runtime_error("Interpteter: unexpected CmpOpKind");
        }
        register_queue_.push(resobj);
        scope_->update_last_exec_res(resobj);
        return;
    }
    if (instanceof<Call>(code)) {
        Call *call = dynamic_cast<Call *>(code);
        if (!instanceof<Name>(call->func().get()) && !instanceof<Attribute>(call->func().get())) {
            throw std::runtime_error("Interpreter: Call.func should be Name");
        }

        if (instanceof<Attribute>(call->func().get())) {
            Attribute *attribute = dynamic_cast<Attribute *>(call->func().get());
            interpret_expr(attribute->value());
            YObject *target = LAST_EXEC_RES_YOBJ;
            if (target->get_typename() != "list") {
                throw std::runtime_error("Interpreter: Currently only list attributes");
            }

            std::string attr = attribute->attr();
            if (attr != "append") {
                throw std::runtime_error("Interpreter: Currently supported only list.append as attribute");
            }
            if (call->args().size() != 1) {
                throw std::runtime_error("Interpreter: list.append require 1 argument");
            }
            interpret_expr(call->args()[0]);

            ManagedObject *arg = LAST_EXEC_RES_M_YOBJ;
            target->add_list_element(arg);
            return;
        }

        std::string func_name = dynamic_cast<Name *>(call->func().get())->id();
        if (func_name == "print") {
            if (call->args().size() != 1) {
                throw std::runtime_error("Interpreter: print can take only 1 argument");
            }
            interpret_expr(call->args()[0]);
            YObject *print_arg = LAST_EXEC_RES_YOBJ;
            if (print_arg->get_typename() != "string") {
                throw std::runtime_error("Interpreter: print argument should be string");
            }
            std::cout << print_arg->get_value_as_string();
            return;
        }
        if (func_name == Scope::yapvm_thread_func_name) {
            if (call->args().size() != 2) {
                throw std::runtime_error("Interpreter: thread should have 2 params - function and args to call");
            }
            if (dynamic_cast<Name *>(call->args()[0].get()) == nullptr) {
                throw std::runtime_error("Interpreter: thread first argument should be function name");
            }
            std::string callee_name = dynamic_cast<Name *>(call->args()[0].get())->id();
            ScopeEntry callee_sce = scope_->name_lookup(Scope::scope_entry_function_name(callee_name));
            if (callee_sce.type_ != FUNCTION) {
                throw std::runtime_error("Interpreterer: cannot find function " + callee_name);
            }
            FunctionDef *callee = static_cast<FunctionDef *>(callee_sce.value_);
            if (callee->args().size() != 1) {
                throw std::runtime_error("Interpreter: thread callee should have only one argument");
            }

            interpret_expr(call->args()[1]);
            ManagedObject *arg = LAST_EXEC_RES_M_YOBJ;

            size_t id;
            do {
                id = GLOBAL_BORN_THREAD_ID.load();
            } while (!GLOBAL_BORN_THREAD_ID.compare_exchange_strong(id, id + 1));
            std::string thread_name = Scope::scope_entry_thread_name(id);

            scope_->change(thread_name, ScopeEntry{ new Scope{scope_}, SCOPE });
            Scope *thread_scope = static_cast<Scope *>(scope_->get(thread_name).value().value_);
            thread_scope->change(callee->args()[0], ScopeEntry{ arg, OBJECT });
            Interpreter *thread = new Interpreter{
                new Module{ copy_vec_no_owning(callee->body()) }, thread_manager_, thread_scope
            };
            thread->launch();

            ManagedObject *resobj = new ManagedObject{
                new YObject{ "int", new ssize_t{ static_cast<ssize_t>(reinterpret_cast<size_t>(thread)) } }
            };
            scope_->update_last_exec_res(resobj);
            Logger::log("Interpreter",
                "created and launched new thread with interpreter ["
                + std::to_string(reinterpret_cast<size_t>(thread))
                + "] from interpreter ["
                + std::to_string(reinterpret_cast<size_t>(this))
                + "]"
            );
            return;
        }
        if (func_name == Scope::yapvm_thread_join_func_name) {
            if (call->args().size() != 1) {
                throw std::runtime_error("Interpreter: thread join can have only one argument");
            }
            interpret_expr(call->args()[0].get());

            YObject *thr_sce_name_object = LAST_EXEC_RES_YOBJ;
            if (thr_sce_name_object->get_typename() != "int") {
                throw std::runtime_error("Interpreter: unrecognized thread object");
            }
            Interpreter *thread = reinterpret_cast<Interpreter *>(
                static_cast<size_t>(thr_sce_name_object->get_value_as_int())
            );

            Logger::log("Interpreter",
                "waiting to join thread with interpreter ["
                + std::to_string(reinterpret_cast<size_t>(thread))
                + "] from interpreter ["
                + std::to_string(reinterpret_cast<size_t>(this))
                + "]"
            );
            std::optional<bool> is_thread_registered_opt = thread_manager_->is_registered(thread);
            while (true) {
                if (is_thread_registered_opt.has_value()) {
                    if (!is_thread_registered_opt.value()) {
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
                is_thread_registered_opt = thread_manager_->is_registered(thread);
                handle_safepoint();
            }
            while (!thread_manager_->finish_waiting()) {
                handle_safepoint();
            }
            handle_safepoint();
            Logger::log("Interpreter",
                "joined thread with interpreter ["
                + std::to_string(reinterpret_cast<size_t>(thread))
                + "] from interpreter ["
                + std::to_string(reinterpret_cast<size_t>(this))
                + "]"
            );
            return;
        }
        //TODO dict
        if (func_name == "str") {
            if (call->args().size() != 1) {
                throw std::runtime_error("Interpreter: str can take only 1 argument");
            }
            interpret_expr(call->args()[0]);
            YObject *arg = LAST_EXEC_RES_YOBJ;

            ManagedObject *resobj = nullptr;
            if (arg->get_typename() == "string") {
                resobj = new ManagedObject{ new YObject{ "string", new std::string{ arg->get_value_as_string() } } };
            } else if (arg->get_typename() == "int") {
                resobj = new ManagedObject{ new YObject{ "string", new std::string{ std::to_string(arg->get_value_as_int()) } } };
            } else if (arg->get_typename() == "float") {
                resobj = new ManagedObject{ new YObject{ "string", new std::string{ std::to_string(arg->get_value_as_float()) } } };
            } else if (arg->get_typename() == "bool") {
                std::string val = "False";
                if (arg->get_value_as_bool()) {
                    val = "True";
                }
                resobj = new ManagedObject{ new YObject{ "string", new std::string{  std::move(val) } } };
            }

            if (resobj == nullptr) {
                throw std::runtime_error("Interpreter: cannot construct string from " + arg->get_typename());
            }
            register_queue_.push(resobj);
            scope_->update_last_exec_res(resobj);
            return;
        }
        if (func_name == "int") {
            if (call->args().size() != 1) {
                throw std::runtime_error("Interpreter: int can take only 1 argument");
            }
            interpret_expr(call->args()[0]);
            YObject *arg = LAST_EXEC_RES_YOBJ;

            ManagedObject *resobj = nullptr;
            if (arg->get_typename() == "string") {
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ from_str<ssize_t>(arg->get_value_as_string()) } } };
            } else if (arg->get_typename() == "int") {
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ arg->get_value_as_int() } } };
            } else if (arg->get_typename() == "float") {
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ static_cast<ssize_t>(arg->get_value_as_float()) } } };
            } else if (arg->get_typename() == "bool") {
                ssize_t val = 0;
                if (arg->get_value_as_bool()) {
                    val = 1;
                }
                resobj = new ManagedObject{ new YObject{ "int", new ssize_t{ val } } };
            }

            if (resobj == nullptr) {
                throw std::runtime_error("Interpreter: cannot construct int from " + arg->get_typename());
            }
            register_queue_.push(resobj);
            scope_->update_last_exec_res(resobj);
            return;
        }
        if (func_name == "float") {
            if (call->args().size() != 1) {
                throw std::runtime_error("Interpreter: float can take only 1 argument");
            }
            interpret_expr(call->args()[0]);
            YObject *arg = LAST_EXEC_RES_YOBJ;

            ManagedObject *resobj = nullptr;
            if (arg->get_typename() == "string") {
                resobj = new ManagedObject{ new YObject{ "float", new double{ from_str<double>(arg->get_value_as_string()) } } };
            } else if (arg->get_typename() == "int") {
                resobj = new ManagedObject{ new YObject{ "float", new double{ static_cast<double>(arg->get_value_as_int()) } } };
            } else if (arg->get_typename() == "float") {
                resobj = new ManagedObject{ new YObject{ "float", new double{ arg->get_value_as_float() } } };
            }

            if (resobj == nullptr) {
                throw std::runtime_error("Interpreter: cannot construct float from " + arg->get_typename());
            }
            register_queue_.push(resobj);
            scope_->update_last_exec_res(resobj);
            return;
        }
        if (func_name == "list") {
            if (!call->args().empty()) {
                throw std::runtime_error("Interpreter: list constructor cannot take arguments");
            }
            ManagedObject *resobj = new ManagedObject{ new YObject{ "list", new std::vector<ManagedObject *>{} } };
            register_queue_.push(resobj);
            scope_->update_last_exec_res(resobj);
            return;
        }

        FunctionDef *function_def = static_cast<FunctionDef *>(
            scope_->name_lookup(Scope::scope_entry_function_name(func_name)).value_
        );
        std::vector<ManagedObject *> call_args;
        for (Expr *e : call->args()) {
            interpret_expr(e);
            call_args.push_back(LAST_EXEC_RES_M_YOBJ);
        }
        std::string scope_name = Scope::scope_entry_call_subscope_name(func_name);
        scope_->change(scope_name, ScopeEntry{ new Scope{ scope_ }, SCOPE });
        scope_ = static_cast<Scope *>(scope_->get(scope_name).value().value_);

        if (call->args().size() != function_def->args().size()) {
            throw std::runtime_error("Interpreter: invalid number of arguments for function " + func_name);
        }
        for (size_t i = 0; i < call->args().size(); i++) {
            scope_->change(function_def->args()[i], ScopeEntry{ call_args[i], OBJECT });
        }
        for (Stmt *stmt : function_def->body()) {
            if (!interpret(stmt)) {
                break;
            }
        }
        if (dynamic_cast<Return *>(function_def->body()[function_def->body().size() - 1].get()) == nullptr) {
            throw std::runtime_error("Interpreter: function should end with return statement");
        }
        // expected that function ended with return -> should some kind of a delete allocated scope
        scope_->del(scope_name); // TODO check
        return;
    }
    if (instanceof<Constant>(code)) {
        Constant *constant = dynamic_cast<Constant *>(code);
        YObject *c_val = constant->value(); //TODO bug probably here with double c_val deletion
        YObject *c_val_cpy = nullptr;
        if (c_val->get_typename() == "bool") {
            c_val_cpy = new YObject{ "bool", new bool{ c_val->get_value_as_bool() } };
        } else if (c_val->get_typename() == "int") {
            c_val_cpy = new YObject{ "int", new ssize_t{ c_val->get_value_as_int() } };
        } else if (c_val->get_typename() == "float") {
            c_val_cpy = new YObject{ "float", new double{ c_val->get_value_as_float() } };
        } else if (c_val->get_typename() == "string") {
            c_val_cpy = new YObject{ "string", new std::string{ c_val->get_value_as_string() } };
        }
        assert(c_val_cpy != nullptr);
        ManagedObject *resobj = new ManagedObject{ c_val_cpy };
        register_queue_.push(resobj);
        scope_->update_last_exec_res(resobj);
        return;
    }
    if (instanceof<Name>(code)) {
        Name *name = dynamic_cast<Name *>(code);
        ScopeEntry n_sce = scope_->name_lookup(name->id());
        if (n_sce.type_ != OBJECT) {
            throw std::runtime_error("Interpreter: " + name->id() + " is not name of object");
        }
        scope_->update_last_exec_res(static_cast<ManagedObject *>(n_sce.value_));
        return;
    }
    if (instanceof<Subscript>(code)) {
        Subscript *subscript = dynamic_cast<Subscript *>(code);

        interpret_expr(subscript->value());
        YObject *value = LAST_EXEC_RES_YOBJ;
        if (value->get_typename() != "list") {
            throw std::runtime_error("Interpreter: Subscript currently supported only for lists");
        }

        interpret_expr(subscript->key());
        YObject *key = LAST_EXEC_RES_YOBJ;
        if (key->get_typename() != "int") {
            throw std::runtime_error("Interpreter: Subscript key for list should be int");
        }

        ssize_t key_v = key->get_value_as_int();
        if (std::abs(key_v) >= value->get_len_as_list()) {
            throw std::runtime_error("Interpreter: list index out of range");
        }
        if (key_v < 0) {
            key_v = static_cast<ssize_t>(key->get_len_as_list()) - key_v;
        }
        ManagedObject *element = value->get_list_element(key_v);
        scope_->update_last_exec_res(element); // TODO check
        return;
    }
    //TODO attribute, subscript

    throw std::runtime_error("Interpreter: unexpected expression");
}


bool yapvm::interpreter::Interpreter::interpret_stmt(Stmt *code) {
    assert(code != nullptr);
    handle_safepoint();

    if (instanceof<Import>(code)) {
        return true; // currently just ignore
    }
    if (instanceof<FunctionDef>(code)) {
        FunctionDef *fdef = dynamic_cast<FunctionDef *>(code);
        scope_->change(Scope::scope_entry_function_name(fdef->name()), ScopeEntry{ fdef, FUNCTION });
        return true;
    }
    if (instanceof<ClassDef>(code)) {
        throw std::runtime_error("Interpreter: ClassDef currently not supported");
    }
    if (instanceof<Return>(code)) {
        Return *rt = dynamic_cast<Return *>(code);

        if (rt->returns_anything()) {
            interpret_expr(rt->value());
        }

        if (scope_ == main_scope_) {
            finishing_.store(true);
            delete main_scope_;
            return false;
        }
        Scope *prev = scope_;
        scope_ = scope_->parent();
        scope_->change(Scope::lst_exec_res, prev->get(Scope::lst_exec_res).value());
        delete prev;
        return false;
    }
    if (instanceof<Assign>(code)) {
        //TODO add assign to subscript
        Assign *assign = dynamic_cast<Assign *>(code);
        if (assign->target().size() != 1 || (!instanceof<Name>(assign->target()[0].get()) && !instanceof<Subscript>(assign->target()[0].get()))) {
            throw std::runtime_error("Interpreter: currently can assign only to single Name");
        }
        if (instanceof<Name>(assign->target()[0].get())) {
            Name *target = dynamic_cast<Name *>(assign->target()[0].get());
            assert(target != nullptr);
            interpret_expr(assign->value());
            scope_->store_last_exec_res(target->id());
        } else if (instanceof<Subscript>(assign->target()[0].get())) {
            Subscript *subscript = dynamic_cast<Subscript *>(assign->target()[0].get());
            assert(subscript != nullptr);

            interpret_expr(subscript->value());
            YObject *value = LAST_EXEC_RES_YOBJ;
            if (value->get_typename() != "list") {
                throw std::runtime_error("Interpreter: currently can assign only to list subscript");
            }
            interpret_expr(subscript->key());
            YObject *key = LAST_EXEC_RES_YOBJ;
            if (key->get_typename() != "int") {
                throw std::runtime_error("Interpreter: list subscript key should be int");
            }
            size_t idx = static_cast<size_t>(key->get_value_as_int());

            interpret_expr(assign->value());
            ManagedObject *assignee = LAST_EXEC_RES_M_YOBJ;
            value->set_list_element(idx, assignee);
            scope_->update_last_exec_res(assignee);
        }
        return true;
    }
    if (instanceof<AugAssign>(code)) {
        AugAssign *aug_assign = dynamic_cast<AugAssign *>(code);

        interpret_expr(aug_assign->target());
        YObject *target = LAST_EXEC_RES_YOBJ;
        if (target->get_typename() != "list") {
            throw std::runtime_error("Interpreter: currently AugAssign supported only for lists");
        }
        if (dynamic_cast<Add *>(aug_assign->op().get()) == nullptr) {
            throw std::runtime_error("Interpreter: currently AugAssign for lists supported only for Add");
        }

        interpret_expr(aug_assign->value());
        ManagedObject *value = LAST_EXEC_RES_M_YOBJ;
        target->add_list_element(value);
        scope_->update_last_exec_res(value);
        return true;
    }
    if (instanceof<While>(code)) {
        While *while_ = dynamic_cast<While *>(code);
        while (true) {
            interpret_expr(while_->test());
            YObject *test_res = LAST_EXEC_RES_YOBJ;
            if (test_res->get_typename() != "bool") {
                throw std::runtime_error("Interpreter: While.test expression should be bool");
            }
            if (!test_res->get_value_as_bool()) {
                break;
            }
            for (Stmt *stmt : while_->body()) {
                if (!interpret_stmt(stmt)) {
                    break;
                }
            }
        }
        return true;
    }
    if (instanceof<For>(code)) {
        throw std::runtime_error("Interpreter: currently only while loops are supported");
    }
    if (instanceof<With>(code)) {
        throw std::runtime_error("Interpreter: currently With are not supported");
    }
    if (instanceof<If>(code)) {
        If *if_ = dynamic_cast<If *>(code);

        interpret_expr(if_->test());
        YObject *test_res = LAST_EXEC_RES_YOBJ;
        if (test_res->get_typename() != "bool") {
            throw std::runtime_error("Interpreter: If.test expression should be bool");
        }
        if (test_res->get_value_as_bool()) {
            for (Stmt *stmt : if_->body()) {
                if (!interpret_stmt(stmt)) {
                    return false;
                }
            }
        } else {
            for (Stmt *stmt : if_->orelse()) {
                if (!interpret_stmt(stmt)) {
                    return false;
                }
            }
        }
        return true;
    }
    if (instanceof<ExprStmt>(code)) {
        interpret_expr(dynamic_cast<ExprStmt *>(code)->value());
        return true;
    }
    if (instanceof<Pass>(code)) {
        return true; // just skip pass instr
    }
    if (instanceof<Continue>(code) || instanceof<Break>(code)) {
        throw std::runtime_error("Interpreter: in current version Break and Continue statements are not supported");
    }

    throw std::runtime_error("Interpreter: unexpected statement");
}


bool yapvm::interpreter::Interpreter::interpret(Node *code) {
    assert(code != nullptr);
    if (instanceof<Stmt>(code)) {
        return interpret_stmt(dynamic_cast<Stmt *>(code));
    }
    throw std::runtime_error("Interpreter: interpret() can deal only with Stmt-s");
}


yapvm::interpreter::Interpreter::Interpreter(scoped_ptr<Module> &&code, ThreadManager *tm, Scope *scope) :
    code_{code.steal()}, scope_{scope}, main_scope_{scope_}, thread_manager_{tm} {
    while (!tm->register_interpreter(this)) {
        handle_safepoint();
    }
}


yapvm::interpreter::Interpreter::~Interpreter() {
    delete code_;
}


void yapvm::interpreter::Interpreter::park() { need_park_.store(true); }


bool yapvm::interpreter::Interpreter::is_parked() const {
    return parked_.load();
}


void yapvm::interpreter::Interpreter::run() {
    need_unpark_.store(true);
}

void yapvm::interpreter::Interpreter::launch() {
    worker_ = std::thread{&Interpreter::__worker_exec, this, code_};
}

bool yapvm::interpreter::Interpreter::is_finished() const { return finished_.load(); }


void yapvm::interpreter::Interpreter::join() { worker_.join(); }

yapvm::interpreter::Scope *yapvm::interpreter::Interpreter::get_scope() const {
    return scope_;
}


std::vector<yapvm::yobjects::ManagedObject *> yapvm::interpreter::Interpreter::get_register_queue() {
    std::vector<ManagedObject *> ret;
    while (!register_queue_.empty()) {
        ret.push_back(register_queue_.top());
        register_queue_.pop();
    }
    return ret;
}
