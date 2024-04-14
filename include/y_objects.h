#pragma once

#include <cstddef>
#include <cstddef>
#include <map>
#include <deque>
#include <functional>
#include <sys/types.h>
#include "utils.h"


#if _MSC_VER
static_assert(sizeof(void *) == 8); // for MSVC x64 compatibility
using ssize_t = long long;
#endif

namespace yapvm {
namespace yobjects {

class YIterator;

class YObject {
protected:
    bool is_hashable_;
    bool is_iterable_;
    bool is_marked_ = false;

public:
    YObject(bool is_hashable, bool is_iterable);
    bool is_hashable() const;
    bool &is_marked();
    virtual YIterator* begin() { return nullptr; };
    virtual YIterator* end() { return nullptr; };
    virtual size_t hash() const {return 0;};
    virtual ~YObject() = default;
};

class YHash {
public:
    size_t operator()(const YObject &value);
};

class YIterator {
public:
	virtual YIterator* next() = 0;
	virtual YObject &operator*() = 0;
	virtual YIterator &operator++() = 0;
	virtual ~YIterator() = default;
};

class YIteratorList : public YIterator {
	std::deque<YObject>::iterator iterator_;
public: 
	YIteratorList(const std::deque<YObject>::iterator &value);
	std::deque<YObject>::iterator& iter();
	YIterator *start();
	YIterator *end();
	YIterator *next();
	YObject &operator*();
	YIterator &operator++();
	~YIteratorList() = default;
};

class YCustomClasses : public YObject {
    std::map<std::string, YObject> dict_;

public:
    YCustomClasses(const std::map<std::string, YObject> &dict);
    // YIterator* begin();
    // YIterator* end();
    // YIterator *iter();
    // size_t hash() const;
};


class YPrimitiveObject : public YObject {
public:
    YPrimitiveObject(bool is_hashable, bool is_iterable) : YObject(is_hashable, is_iterable) {};
};

class YNoneObject : public YPrimitiveObject {};

class YBoolObject : public YPrimitiveObject {
    bool value_;

public:
    YBoolObject(bool value);
    bool value() const;
    size_t hash() { return (value_) ? 1 : 0;}
};

class YStringObject : public YPrimitiveObject {
    std::string value_;

public:
    YStringObject(const std::string &value);
    const std::string &value() const;
    size_t hash() { return std::hash<std::string>{}(value_); }
};

class YFloatObject : public YPrimitiveObject {
    double value_;

public:
    YFloatObject(double value);
    double value() const;
    size_t hash() { return std::hash<double>{}(value_); }
};

class YIntObject : public YPrimitiveObject {
    ssize_t value_;

public:
    YIntObject(ssize_t value);
    ssize_t value() const;
    size_t hash() { return std::hash<ssize_t>{}(value_); }
};


class YListObject : public YObject {
    std::deque<YObject> list_;

public:
    YListObject() : YObject{ false, true }, list_(std::deque<YObject> {}) {};

    YListObject(const std::deque<YObject> &value);
    YListObject(const std::vector<YObject> &value);

    YListObject(const YListObject &value);

    YListObject &operator=(const YListObject &other);

    YIterator* begin();
    YIterator* end();

    std::deque<YObject> &value();

    const YObject &operator[](size_t index) const;

    YListObject operator+(const YListObject &other);

    void append(const YObject &val);

    size_t size() const;
};

class YTupleObject : public YObject {
    std::vector<YObject> tuple_;

public:
    YTupleObject(const std::vector<YObject> &value);

    YTupleObject(const YTupleObject &other);
    YTupleObject &operator=(const YTupleObject &other);

    const YObject &operator[](size_t index) const;

    YIterator *begin();
    YIterator *end();

    size_t size() const;
    size_t hash();
};
/*
class YDictObject : public YObject {
    std::unordered_map<YObject, YObject, YHash> dict_;

public:
    YDictObject() : YObject { false, true } {};
    void add(const YObject &key, YObject &value);
    YObject &get(const YObject &key);
    YDictObject(const YTupleObject &other);
    YDictObject &operator=(const YTupleObject &other);

    const YObject &operator[](size_t index) const;

    YIterator *begin();
    YIterator *end();

    size_t size() const;
};
*/

}
}
