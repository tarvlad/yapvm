#pragma once

#include <string>
#include <cstddef>
#include <cstddef>

#if _MSC_VER
static_assert(sizeof(void *) == 8); // for MSVC x64 compatibility
using ssize_t = long long;
#endif

namespace yapvm {
namespace yobjects {


class YObject {

public:

    virtual ~YObject() = default;
};


class YPrimitiveObject : public YObject {};

class YNoneObject : public YPrimitiveObject {};

class YBoolObject : public YPrimitiveObject {
    bool value_;

public:
    YBoolObject(bool value);
    bool value() const;
};

class YStringObject : public YPrimitiveObject {
    std::string value_;

public:
    YStringObject(const std::string &value);
    const std::string &value() const;
};

class YFloatObject : public YPrimitiveObject {
    double value_;

public:
    YFloatObject(double value);
    double value() const;
};

class YIntObject : public YPrimitiveObject {
    ssize_t value_;

public:
    YIntObject(ssize_t value);
    ssize_t value() const;
};

class YListObject : public YObject {
    std::deque<YObject> list_;

public:
    YListObject() = default;
    YListObject(const std::deque<YObject> &value);
    YListObject(const std::vector<YObject> &value);
    YListObject(const YListObject &value);
    YListObject &operator=(const YListObject &other);
    ~YListObject() = default;
    const std::deque<YObject> &value() const;
    const YObject &operator[](size_t index) const;
    YListObject operator+(const YListObject &other);

    void append(const YObject &val);
    size_t size() const;
};

class YTupleObject : public YObject {
    array<YObject> tuple_;

public:
    YTupleObject(size_t size);
    YTupleObject(const std::vector<YObject> &value);
    YTupleObject(const YTupleObject &value);
    YTupleObject &operator=(const YTupleObject &other);
    ~YTupleObject() = default;
    const YObject &operator[](size_t index) const;
    size_t size() const;
};

/* Broken dict
class YDictObject : public YObject {
    std::unordered_map<YObject, YObject> dict_;

public: 
    YDictObject() = default;
    ~YDictObject() = default;
    YDictObject(const YDictObject &value);
    YDictObject &operator=(const YDictObject &other);
    const YObject &operator[](const YObject &key) const;
    void add(const YObject &key, const YObject &value);
    const YObject &get(const YObject &key) const;

};
*/
}
}