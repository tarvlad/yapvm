#include "interpreter.h"
#include <chrono>
#include <cmath>


// TODO rewrite as Scope method
#define LAST_EXEC_RES_YOBJ static_cast<ManagedObject *>(scope_->get(Scope::lst_exec_res).value().value_)->value()

// SHOULD be called only once when interpreter starts
void yapvm::interpreter::Interpreter::__worker_exec(Module *code) {
    while (!resuming_.load()) {
        std::this_thread::sleep_for(std::chrono::nanoseconds{500});
    }

    for (const scoped_ptr<Stmt> &i: code->body()) {
        interpret(i);
        //TODO check and park (handle unpark too)
        if (finishing_.load()) {
            break;
        }
    }
    finishing_.store(true);
    resuming_.store(false);
    parked_.store(true);
    stopping_.store(false);
    finished_.store(true);
    finishing_.store(false);
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

        if (left->get_typename() != right->get_typename()) {
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

    throw std::runtime_error("Interpreter: unexpected expression");
}


void yapvm::interpreter::Interpreter::interpret_stmt(Stmt *code) {
    assert(code != nullptr);

    if (instanceof<Import>(code)) {
        return; // currently just ignore
    }
    if (instanceof<FunctionDef>(code)) {
        FunctionDef *fdef = dynamic_cast<FunctionDef *>(code);
        scope_->add_function(fdef->name(), fdef);
        return;
    }
    if (instanceof<ClassDef>(code)) {
        throw std::runtime_error("Interpreter: ClassDef currently not supported");
    }
    if (instanceof<Return>(code)) {
        Return *rt = dynamic_cast<Return *>(code);

        while (!scope_->get(Scope::function_ret_label).has_value()) {
            scope_ = scope_->parent();
        }

        if (scope_ == main_scope_) {
            finishing_.store(true);
            return;
        }
        if (rt->returns_anything()) {
            interpret_expr(rt->value());
        }
        Scope *prev = scope_;
        scope_ = scope_->parent();
        scope_->change(Scope::lst_exec_res, prev->get(Scope::lst_exec_res).value());
        delete prev;
        return;
    }
    if (instanceof<Assign>(code)) {
        Assign *assign = dynamic_cast<Assign *>(code);
        if (assign->target().size() != 1 || !instanceof<Name>(assign->target()[0].get())) {
            throw std::runtime_error("Interpreter: currently can assign only to single Name");
        }
        Name *target = dynamic_cast<Name *>(assign->target()[0].get());
        assert(target != nullptr);
        interpret_expr(assign->value());
        scope_->store_last_exec_res(target->id());
        return;
    }
    if (instanceof<AugAssign>(code)) {
        throw std::runtime_error("Interpreter: currently AugAssign is not supported");
    }
    if (instanceof<While>(code)) {
        While *while_ = dynamic_cast<While *>(code);
        scope_->change(Scope::while_loop_scope, ScopeEntry{ new Scope{ scope_ }, SCOPE });
        scope_ = static_cast<Scope *>(scope_->get(Scope::while_loop_scope).value().value_);

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
                interpret_stmt(stmt);
            }
        }
        return;
    }
    if (instanceof<For>(code)) {
        throw std::runtime_error("Interpreter: currently only while loops are supported");
    }
    if (instanceof<With>(code)) {
        throw std::runtime_error("Interpreter: currently With are not supported");
    }
    if (instanceof<If>(code)) {
        If *if_ = dynamic_cast<If *>(code);
        scope_->change(Scope::if_scope, ScopeEntry{ new Scope{ scope_ }, SCOPE });
        scope_ = static_cast<Scope *>(scope_->get(Scope::if_scope).value().value_);

        interpret_expr(if_->test());
        YObject *test_res = LAST_EXEC_RES_YOBJ;
        if (test_res->get_typename() != "bool") {
            throw std::runtime_error("Interpreter: If.test expression should be bool");
        }
        if (test_res->get_value_as_bool()) {
            for (Stmt *stmt : if_->body()) {
                interpret_stmt(stmt);
            }
        } else {
            for (Stmt *stmt : if_->orelse()) {
                interpret_stmt(stmt);
            }
        }
        return;
    }
    if (instanceof<ExprStmt>(code)) {
        interpret_expr(dynamic_cast<ExprStmt *>(code)->value());
    }
    if (instanceof<Pass>(code)) {
        return; // just skip pass instr
    }
    if (instanceof<Continue>(code) || instanceof<Break>(code)) {
        throw std::runtime_error("Interpreter: in current version Break and Continue statements are not supported");
    }

    throw std::runtime_error("Interpreter: unexpected statement");
}


void yapvm::interpreter::Interpreter::interpret(Node *code) {
    assert(code != nullptr);
    if (instanceof<Stmt>(code)) {
        interpret_stmt(dynamic_cast<Stmt *>(code));
        return;
    }
    throw std::runtime_error("Interpreter: interpret() can deal only with Stmt-s");
}


yapvm::interpreter::Interpreter::Interpreter(scoped_ptr<Module> &&code)
    : code_{code.steal()}, scope_{new Scope{}}, main_scope_{scope_}, worker_{&Interpreter::__worker_exec, this, code_} {
    main_scope_->add(Scope::function_ret_label, ScopeEntry{ nullptr, LABEL });
}


yapvm::interpreter::Interpreter::~Interpreter() { delete code_; }


void yapvm::interpreter::Interpreter::park() { stopping_.store(true); }


bool yapvm::interpreter::Interpreter::is_parked() const {
    return parked_.load();
}


void yapvm::interpreter::Interpreter::run() {
    while (stopping_.load()) {
    } // wait while thread parks
    resuming_.store(true); // all {stopping <- true} actions should hb {resuming <- true} actions
}

bool yapvm::interpreter::Interpreter::is_finished() const { return finished_.load(); }


void yapvm::interpreter::Interpreter::join() {
    worker_.join();
}


std::vector<yapvm::yobjects::ManagedObject *> yapvm::interpreter::Interpreter::get_register_queue() {
    std::vector<ManagedObject *> ret;
    while (!register_queue_.empty()) {
        ret.push_back(register_queue_.top());
        register_queue_.pop();
    }
    return ret;
}
