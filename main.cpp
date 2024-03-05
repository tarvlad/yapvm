#include <print>
#include "Utils.h"
#include "Function.h"
#include "Parser.h"
#include "Module.h"

using namespace yapvm;

int main(int argc, char **argv) {
    if (argc != 2) {
        std::print("Need input filename");
        return 0;
    }

    std::string filename = argv[1]; //TODO check
    std::string file_content = read_file(filename);

    Module code = parse(filename, file_content);
    file_content.erase();
    //TODO
}
