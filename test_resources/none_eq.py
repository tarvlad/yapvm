import ast
import inspect


def __eq__(self, other):
    return type(self) == type(other)


print(ast.dump(ast.parse(inspect.getsource(__eq__))))
