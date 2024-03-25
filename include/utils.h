#pragma once

#include <string>
#include <sstream>


namespace yapvm {

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

template<typename T, __scptr_copy_semantics CopySem = COPY>
class scoped_ptr {
private:
    T* ptr_;

public:
    scoped_ptr(T* p = nullptr) : ptr_{ p } {}

    ~scoped_ptr() {
        delete ptr_;
    }

    // Disallow copying if specified as Move
    template<__scptr_copy_semantics C = CopySem>
    scoped_ptr(const scoped_ptr<T, C>&) requires (CopySem == __scptr_copy_semantics::COPY) = delete;

    template<__scptr_copy_semantics C = CopySem>
    scoped_ptr& operator=(const scoped_ptr<T, C>&) requires (CopySem == __scptr_copy_semantics::COPY) = delete;

    // Move constructor
    scoped_ptr(scoped_ptr &&other) noexcept {
        if constexpr (CopySem == __scptr_copy_semantics::MOVE) {
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        } else {
            ptr_ = new T(*other.ptr_);
            other.ptr_ = nullptr;
        }
    }

    // Move assignment
    scoped_ptr& operator=(scoped_ptr &&other) noexcept {
        if (this != &other) {
            if constexpr (CopySem == __scptr_copy_semantics::MOVE) {
                delete ptr_;
                ptr_ = other.ptr_;
                other.ptr_ = nullptr;
            } else {
                if (ptr_ != other.ptr_) {
                    delete ptr_;
                    ptr_ = new T(*other.ptr_);
                }
                other.ptr_ = nullptr;
            }
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

    void reset(T *p = nullptr) {
        if constexpr (CopySem == __scptr_copy_semantics::COPY) {
            delete ptr_;
            ptr_ = (p != nullptr) ? new T(*p) : nullptr;
        } else {
            delete ptr_;
            ptr_ = p;
        }
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }
};


}