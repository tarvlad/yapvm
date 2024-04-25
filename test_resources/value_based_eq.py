import ast
import inspect


def __eq__(self, other):
    return type(self) == type(other) and self.___yapvm_objval_ == self.___yapvm_objval_


print(ast.dump(ast.parse(inspect.getsource(__eq__))))
