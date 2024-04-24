#include "utils.h"
#include <sstream>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <string>

using namespace yapvm;


std::string yapvm::read_file(const std::string &s) {
    std::ifstream t{ s };
    assert(t.is_open());

    std::stringstream buffer;

    buffer << t.rdbuf();
    return buffer.str();
}


static
bool __sstrcmp(const char *expr, size_t expr_size, const char *pattern, size_t pattern_size, size_t offset) {
    if (pattern_size == 0) {
        return true;
    }
    if (expr_size < pattern_size) {
        return false;
    }
    if (pattern_size + offset > expr_size) {
        return false;
    }

    size_t e_ctr = offset;
    size_t p_ctr = 0;
    do {
        if (expr[e_ctr] != pattern[p_ctr]) {
            return false;
        }
        e_ctr++;
        p_ctr++;
    } while (p_ctr < pattern_size);

    return true;
}


bool yapvm::sstrcmp(const std::string &expr, const char *pattern, size_t offset) {
    return __sstrcmp(expr.c_str(), expr.size(), pattern, cstrsz(pattern), offset);
}


bool yapvm::sstrcmp(const std::string &expr, const std::string &pattern, size_t offset) {
    return __sstrcmp(expr.c_str(), expr.size(), pattern.c_str(), pattern.size(), offset);
}


bool yapvm::sstrcmp(const char *expr, const std::string &pattern, size_t offset) {
    return __sstrcmp(expr, cstrsz(expr), pattern.c_str(), pattern.size(), offset);
}


bool yapvm::sstrcmp(const char *expr, const char *pattern, size_t offset) {
    return __sstrcmp(expr, cstrsz(expr), pattern, cstrsz(pattern), offset);
}


size_t yapvm::cstrsz(const char *str) {
    size_t sz = 0;
    while (str[sz] != 0) {
        sz++;
    }
    return sz;
}


__CheckResObj yapvm::check(bool cond) { return __CheckResObj{cond}; }


std::string yapvm::exec(const std::string &s) {
    const char *cmd = s.c_str();
    std::string result;
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
        throw std::runtime_error("exec(const char *) error");

    try {
        char buffer[128];
        while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


std::string yapvm::trim(const std::string &s) {
    if (s.size() == 0) {
        return "";
    }

    size_t i = s.size() - 1;
    while (true) {
        if (s[i] != ' ' && s[i] != '\n' && s[i] != '\r' && s[i] != '\t') {
            break;
        }
        if (i == 0) {
            break;
        }
        i--;
    }
    return s.substr(0, i + 1);
}


std::string yapvm::extract_delimited_substring(const std::string &str, size_t pos) {
    char delimiter = str[pos];
    if (delimiter != '\'' && delimiter != '"') {
        throw std::invalid_argument("Invalid starting delimiter");
    }

    bool escaped = false;
    size_t end_pos = std::string::npos;
    for (size_t i = pos + 1; i < str.size(); i++) {
        if (str[i] == '\\') {
            escaped = !escaped;
            continue;
        }
        if (escaped) {
            escaped = false;
            continue;
        }
        if (str[i] == delimiter) {
            end_pos = i - 1;
            break;
        }
    }
    if (end_pos == std::string::npos) {
        throw std::invalid_argument("Not <<...>>");
    }
    std::string result = str.substr(pos + 1, end_pos - pos + 1 - 1);
    return result;
}

size_t yapvm::combine_hashes(size_t h1, size_t h2) {
    return h1 ^ h2 << 1;
}
