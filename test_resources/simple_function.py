import ast
import inspect


def foo():
    return "foo"


print(ast.dump(ast.parse(inspect.getsource(foo))))
