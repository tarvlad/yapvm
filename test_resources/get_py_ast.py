import ast
import sys

print(ast.dump(ast.parse(open(sys.argv[1]).read())))