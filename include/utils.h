#pragma once

#include <string>


namespace yapvm {

std::string read_file(const std::string &s);

template <size_t N, typename T>
bool is_there_n_elements_in(const T &container, size_t pos) {
    return pos <= container.size() - N;
}

}