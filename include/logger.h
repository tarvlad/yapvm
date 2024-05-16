#pragma once
#include <string>
#include <fstream>

namespace yapvm {

class Logger {
    constexpr static bool enabled = true;
    constexpr static std::string out_fname = "log.txt";

    static std::ofstream out;
public:
    static void init_logger();

    static void log(std::string msg);

    static void log(std::string who, std::string msg);

    static void log(std::string who, std::string where, std::string msg);
};

}