#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <algorithm>
#include <concepts>


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
    scoped_ptr(scoped_ptr &&other) {
        if constexpr (CopySem == __scptr_copy_semantics::MOVE) {
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        } else {
            ptr_ = new T(*other.ptr_);
            other.ptr_ = nullptr;
        }
    }

    // Move assignment 
    scoped_ptr& operator=(scoped_ptr &&other) {
        if (this != &other) {
            if constexpr (CopySem == __scptr_copy_semantics::MOVE) {
                delete ptr_;
                ptr_ = other.ptr_;
                other.ptr_ = nullptr;
            } else {
                if (ptr_ != other.ptr_) {
                    delete ptr_;
                    ptr_ = new T{*other.ptr_};
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
            ptr_ = (p != nullptr) ? new T{*p} : nullptr;
        } else {
            delete ptr_;
            ptr_ = p;
        }
    }

    operator bool() const {
        return ptr_ != nullptr;
    }
};


template<typename T, typename Allocator = std::allocator<T>>
class array {
private:
    T *data_;
    size_t size_;
    Allocator allocator_;

public:
    array(size_t size, const Allocator &allocator = Allocator()) 
        : data_{allocator.allocate(size)}, size_{size}, allocator_{allocator} {}

    array(const array &other) 
        : data_{allocator_.allocate(other.size_)}, size_{other.size_}, allocator_{other.allocator_} {
        std::uninitialized_copy(other.begin(), other.end(), begin());
    }

    array &operator=(const array &other) {
        if (this != &other) {
            array temp(other);
            swap(temp);
        }
        return *this;
    }

    array(array &&other) noexcept 
        : data_{other.data_}, size_{other.size_}, allocator_{std::move(other.allocator_)} {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    array &operator=(array &&other) noexcept {
        if (this != &other) {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(allocator_, other.allocator_);
        }
        return *this;
    }

    ~array() {
        if (data_) {
            std::destroy(begin(), end());
            allocator_.deallocate(data_, size_);
        }
    }

    T &operator[](size_t index) {
        return data_[index];
    }

    const T &operator[](size_t index) const {
        return data_[index];
    }

    size_t size() const noexcept {
        return size_;
    }

    T *begin() noexcept {
        return data_;
    }

    const T *begin() const noexcept {
        return data_;
    }

    T *end() noexcept {
        return data_ + size_;
    }

    const T *end() const noexcept {
        return data_ + size_;
    }

    void swap(array &other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(allocator_, other.allocator_);
    }
};



}