#pragma once

#include <string>
#include <cstddef>
#include <cstddef>

static_assert(sizeof(void *) == 8);
using ssize_t = unsigned long long;

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

}
}