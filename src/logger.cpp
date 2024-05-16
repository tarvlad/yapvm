#include "logger.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>

std::ofstream yapvm::Logger::out;

static std::string thread_id_str() {
    std::thread::id id = std::this_thread::get_id();
    std::stringstream buf;
    buf << id;
    size_t ret;
    buf >> ret;
    return std::to_string(ret);
}

void yapvm::Logger::init_logger() {
    out = std::ofstream(out_fname);
    assert(out.is_open());
}


void yapvm::Logger::log(std::string msg) {
    if (enabled && out.is_open()) {
        std::string log_msg = std::string("YAPVM (thread ") + thread_id_str() + std::string("): ") + std::move(msg);
        out << log_msg << std::endl;
    }
}

void yapvm::Logger::log(std::string who, std::string msg) {
    if (enabled && out.is_open()) {
        std::string log_msg = std::string("YAPVM (thread ") + thread_id_str() + std::string("): ") + "[" +
                              std::move(who) + "]: " + std::move(msg);
        out << log_msg << std::endl;
    }
}

void yapvm::Logger::log(std::string who, std::string where, std::string msg) {
    if (enabled && out.is_open()) {
        std::string log_msg = std::string("YAPVM (thread ") + thread_id_str() + std::string("): ") + "[" +
                              std::move(who) + "][" + std::move(where) + "]: " + std::move(msg);
        out << log_msg << std::endl;
    }
}
