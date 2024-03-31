#include "ast.h"
#include "parser.h"

using namespace yapvm::ast;
using namespace yapvm;
using namespace yapvm::parser;


int main() {
    std::string test_data = "Module(body=[Import(names=[alias(name='threading')]), FunctionDef(name='dot_product_chunk', args=arguments(posonlyargs=[], args=[arg(arg='start'), arg(arg='end'), arg(arg='vector1'), arg(arg='vector2'), arg(arg='result'), arg(arg='lock')], kwonlyargs=[], kw_defaults=[], defaults=[]), body=[Assign(targets=[Name(id='chunk_result', ctx=Store())], value=Constant(value=0)), For(target=Name(id='i', ctx=Store()), iter=Call(func=Name(id='range', ctx=Load()), args=[Name(id='start', ctx=Load()), Call(func=Name(id='min', ctx=Load()), args=[Name(id='end', ctx=Load()), Call(func=Name(id='len', ctx=Load()), args=[Name(id='vector1', ctx=Load())], keywords=[])], keywords=[])], keywords=[]), body=[AugAssign(target=Name(id='chunk_result', ctx=Store()), op=Add(), value=BinOp(left=Subscript(value=Name(id='vector1', ctx=Load()), slice=Name(id='i', ctx=Load()), ctx=Load()), op=Mult(), right=Subscript(value=Name(id='vector2', ctx=Load()), slice=Name(id='i', ctx=Load()), ctx=Load())))], orelse=[]), With(items=[withitem(context_expr=Name(id='lock', ctx=Load()))], body=[AugAssign(target=Subscript(value=Name(id='result', ctx=Load()), slice=Constant(value=0), ctx=Store()), op=Add(), value=Name(id='chunk_result', ctx=Load()))])], decorator_list=[], type_params=[]), FunctionDef(name='dot_product', args=arguments(posonlyargs=[], args=[arg(arg='vector1'), arg(arg='vector2')], kwonlyargs=[], kw_defaults=[], defaults=[]), body=[Assign(targets=[Name(id='vector_size', ctx=Store())], value=Call(func=Name(id='len', ctx=Load()), args=[Name(id='vector1', ctx=Load())], keywords=[])), If(test=Compare(left=Name(id='vector_size', ctx=Load()), ops=[LtE()], comparators=[Constant(value=512)]), body=[Return(value=Call(func=Name(id='sum', ctx=Load()), args=[GeneratorExp(elt=BinOp(left=Subscript(value=Name(id='vector1', ctx=Load()), slice=Name(id='i', ctx=Load()), ctx=Load()), op=Mult(), right=Subscript(value=Name(id='vector2', ctx=Load()), slice=Name(id='i', ctx=Load()), ctx=Load())), generators=[comprehension(target=Name(id='i', ctx=Store()), iter=Call(func=Name(id='range', ctx=Load()), args=[Name(id='vector_size', ctx=Load())], keywords=[]), ifs=[], is_async=0)])], keywords=[]))], orelse=[Assign(targets=[Name(id='num_threads', ctx=Store())], value=Constant(value=8)), Assign(targets=[Name(id='chunk_size', ctx=Store())], value=BinOp(left=Name(id='vector_size', ctx=Load()), op=FloorDiv(), right=Name(id='num_threads', ctx=Load()))), Assign(targets=[Name(id='threads', ctx=Store())], value=List(elts=[], ctx=Load())), Assign(targets=[Name(id='result', ctx=Store())], value=List(elts=[Constant(value=0)], ctx=Load())), Assign(targets=[Name(id='lock', ctx=Store())], value=Call(func=Attribute(value=Name(id='threading', ctx=Load()), attr='Lock', ctx=Load()), args=[], keywords=[])), For(target=Name(id='i', ctx=Store()), iter=Call(func=Name(id='range', ctx=Load()), args=[Name(id='num_threads', ctx=Load())], keywords=[]), body=[Assign(targets=[Name(id='start', ctx=Store())], value=BinOp(left=Name(id='i', ctx=Load()), op=Mult(), right=Name(id='chunk_size', ctx=Load()))), Assign(targets=[Name(id='end', ctx=Store())], value=IfExp(test=Compare(left=Name(id='i', ctx=Load()), ops=[Lt()], comparators=[BinOp(left=Name(id='num_threads', ctx=Load()), op=Sub(), right=Constant(value=1))]), body=BinOp(left=BinOp(left=Name(id='i', ctx=Load()), op=Add(), right=Constant(value=1)), op=Mult(), right=Name(id='chunk_size', ctx=Load())), orelse=Name(id='vector_size', ctx=Load()))), Assign(targets=[Name(id='thread', ctx=Store())], value=Call(func=Attribute(value=Name(id='threading', ctx=Load()), attr='Thread', ctx=Load()), args=[], keywords=[keyword(arg='target', value=Name(id='dot_product_chunk', ctx=Load())), keyword(arg='args', value=Tuple(elts=[Name(id='start', ctx=Load()), Name(id='end', ctx=Load()), Name(id='vector1', ctx=Load()), Name(id='vector2', ctx=Load()), Name(id='result', ctx=Load()), Name(id='lock', ctx=Load())], ctx=Load()))])), Expr(value=Call(func=Attribute(value=Name(id='threads', ctx=Load()), attr='append', ctx=Load()), args=[Name(id='thread', ctx=Load())], keywords=[])), Expr(value=Call(func=Attribute(value=Name(id='thread', ctx=Load()), attr='start', ctx=Load()), args=[], keywords=[]))], orelse=[]), For(target=Name(id='thread', ctx=Store()), iter=Name(id='threads', ctx=Load()), body=[Expr(value=Call(func=Attribute(value=Name(id='thread', ctx=Load()), attr='join', ctx=Load()), args=[], keywords=[]))], orelse=[]), Return(value=Subscript(value=Name(id='result', ctx=Load()), slice=Constant(value=0), ctx=Load()))])], decorator_list=[], type_params=[]), Assign(targets=[Name(id='vector1', ctx=Store())], value=BinOp(left=List(elts=[Constant(value=1), Constant(value=2), Constant(value=3), Constant(value=4), Constant(value=5)], ctx=Load()), op=Mult(), right=Constant(value=200))), Assign(targets=[Name(id='vector2', ctx=Store())], value=BinOp(left=List(elts=[Constant(value=5), Constant(value=4), Constant(value=3), Constant(value=2), Constant(value=1)], ctx=Load()), op=Mult(), right=Constant(value=200))), Assign(targets=[Name(id='result', ctx=Store())], value=Call(func=Name(id='dot_product', ctx=Load()), args=[Name(id='vector1', ctx=Load()), Name(id='vector2', ctx=Load())], keywords=[])), Expr(value=Call(func=Name(id='print', ctx=Load()), args=[Name(id='result', ctx=Load())], keywords=[]))], type_ignores=[])";
    scoped_ptr<Module> ast = generate_ast(test_data);
}