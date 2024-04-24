#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <functional>


namespace yapvm {

#define RETURN_IF(PREDICATE, VAL) \
if (PREDICATE) { return VAL; }

std::string read_file(const std::string &s);

template <size_t N, typename T>
bool is_there_at_least_n_elements_in(const T &container, size_t pos) {
    return pos <= container.size() - N;
}

template <typename T>
T from_str(const std::string &s) {
    std::stringstream ss{ s };
    T val;
    ss >> val;
    return val;
}

enum __scptr_copy_semantics {
    COPY,
    MOVE
};


class __CheckResObj {
    bool res_;

public:
    __CheckResObj(bool res) : res_{ res } {}

    __CheckResObj &and_then(auto callable) {
        if (res_) {
            callable();
        }
        return *this;
    }

    __CheckResObj &or_else(auto callable) {
        if (!res_) {
            callable();
        }
        return *this;
    }

    auto transform(auto callable) {
        if (res_) {
            return callable();
        }
        return std::nullopt;
    }

    operator bool() const {
        return res_;
    }
};

__CheckResObj check(bool cond);

bool sstrcmp(const std::string &expr, const std::string &pattern, size_t offset);
bool sstrcmp(const std::string &expr, const char *pattern, size_t offset);
bool sstrcmp(const char *expr, const std::string &pattern, size_t offset);
bool sstrcmp(const char *expr, const char *pattern, size_t offset);

size_t cstrsz(const char *str);


template<typename T>
class scoped_ptr {
    T *ptr_;

public:
    scoped_ptr(T *p = nullptr) : ptr_{ p } {}

    ~scoped_ptr() {
        delete ptr_;
    }

    scoped_ptr(const scoped_ptr &) = delete;

    scoped_ptr(scoped_ptr &&p) noexcept : ptr_{ p.ptr_ } {
        p.ptr_ = nullptr;
    }

    scoped_ptr &operator=(const scoped_ptr &) = delete;

    scoped_ptr &operator=(scoped_ptr &&p) noexcept {
        if (&p != this) {
            delete ptr_;
            ptr_ = p.ptr_;
            p.ptr_ = nullptr;
        }
        return *this;
    }

    T *get() const {
        return ptr_;
    }

    T &operator*() const {
        return *ptr_;
    }

    T *operator->() const {
        return ptr_;
    }

    bool operator==(T *p) const {
        return ptr_ == p;
    }

    operator bool() const {
        return ptr_ != nullptr;
    }

    operator T *() const {
        return ptr_;
    }

    T *steal() noexcept {
        T *res = ptr_;
        ptr_ = nullptr;
        return res;
    }
};

template <typename T, typename W, typename Callable, typename... Args>
scoped_ptr<W> conv_or(scoped_ptr<T> &&s, Callable call_if_error, Args&&... args) {
    T *t_val = s.steal();
    W *w_val = dynamic_cast<W *>(t_val);
    if (w_val == nullptr) {
        std::invoke(call_if_error, std::forward<Args>(args)...);
    }
    return w_val;
}


template <typename From, typename To, typename Callable, typename... Args>
To *checked_cast(From *from, Callable call_if_error, Args&& ... args) {
    To *to = dynamic_cast<To *>(from);
    if (to == nullptr) {
        std::invoke(call_if_error, std::forward<Args>(args)...);
    }
    return to;
}


template<typename Callable, typename... Args>
void assume(bool cond, Callable call_if_error, Args&&... args) {
    if (!cond) {
        std::invoke(call_if_error, std::forward<Args>(args)...);
    }
}


std::string exec(const std::string &s);


std::string trim(const std::string &s);


std::string extract_delimited_substring(const std::string &str, size_t pos);

size_t combine_hashes(size_t h1, size_t h2);

}
