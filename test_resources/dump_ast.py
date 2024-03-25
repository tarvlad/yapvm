import ast
import sys


if __name__ == '__main__':
    fname = sys.argv[1]
    
    file = open(fname)
    file_content = file.read()
    file.close()

    ast_dump = ast.dump(ast.parse(file_content))
    print(ast_dump)
