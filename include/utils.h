#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <algorithm>
#include <concepts>
#include <type_traits>


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


template<typename T, __scptr_copy_semantics CopySem = COPY>
class scoped_ptr {
private:
    T *ptr_;

public:
    scoped_ptr(T *p = nullptr) : ptr_{ p } {}

    ~scoped_ptr() {
        delete ptr_;
    }

    scoped_ptr(const scoped_ptr &) requires(CopySem == MOVE) = delete;

    scoped_ptr(const scoped_ptr &p) requires(CopySem == COPY) : ptr_{ new T{ *p } } {}

    scoped_ptr(scoped_ptr &&p) : ptr_{ p.ptr_ } {
        p.ptr_ = nullptr;
    }

    scoped_ptr &operator=(const scoped_ptr &) requires(CopySem == MOVE) = delete;

    scoped_ptr &operator=(const scoped_ptr &p) requires(CopySem == COPY) {
        if (&p != this) {
            delete ptr_;
            ptr_ = new T{ *p };
        }
        return *this;
    }

    scoped_ptr &operator=(scoped_ptr &&p) {
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
};


template<typename T, typename Allocator = std::allocator<T>>
class array {
private:
    Allocator allocator_;
    T *data_;
    size_t size_;

public:
    array(const Allocator &allocator = Allocator{})
        : data_{ nullptr }, size_{ 0 }, allocator_{ allocator } {}

    array(size_t size, const Allocator &allocator = Allocator{})
        : data_{ allocator_.allocate(size) }, size_{ size }, allocator_{ allocator } {
        for (size_t i = 0; i < size_; ++i) {
            if constexpr (std::is_fundamental_v<T>) {
                data_[i] = T{};  // Directly initialize primitive types to zero
            } else {
                std::construct_at(data_ + i);  // Use std::construct_at for non-primitive types
            }
        }
    }

    array(const array &other)
        : data_{ allocator_.allocate(other.size_) }, size_{ other.size_ }, allocator_{ other.allocator_ } {
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
        : data_{ other.data_ }, size_{ other.size_ }, allocator_{ std::move(other.allocator_) } {
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
            for (size_t i = 0; i < size_; ++i) {
                std::destroy_at(data_ + i);  // Use std::destroy_at
            }
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


std::string extract_delimited_substring(const std::string &str, size_t pos);

}