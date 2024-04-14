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
    // void mark();
    virtual YIterator *iter() { return nullptr; };
    virtual size_t hash() const {return 0;};
    virtual bool operator==(const YObject &other) const {return 0;};
    virtual ~YObject() = default;
};


class YHash {
public:
    size_t operator()(const YObject &value);
};

class YIterator {
public:
    virtual YObject &operator*() = 0;
    virtual YIterator &operator++() = 0;
    virtual void next() = 0;
    virtual bool has_next() = 0;
    virtual ~YIterator() = default;
};

class YIteratorCustomClass : public YIterator {
    std::unordered_map<std::string, YObject*>::iterator begin_;
    std::unordered_map<std::string, YObject*>::iterator end_;

public: 
    YIteratorCustomClass(const std::unordered_map<std::string, YObject*>::iterator &begin, const std::unordered_map<std::string, YObject*>::iterator &end);
    bool has_next();
    YObject &operator*();
    YIterator &operator++();
    void next();
};

class YIteratorList : public YIterator {
    std::deque<YObject>::iterator begin_;
    std::deque<YObject>::iterator end_;

public: 
    YIteratorList(const std::deque<YObject>::iterator &begin, const std::deque<YObject>::iterator &end);
    bool has_next();
    YObject &operator*();
    YIterator &operator++();
    void next();
};

class YIteratorTuple : public YIterator {
    std::vector<YObject>::iterator begin_;
    std::vector<YObject>::iterator end_;

public: 
    YIteratorTuple(const std::vector<YObject>::iterator &begin, const std::vector<YObject>::iterator &end);
    bool has_next();
    YObject &operator*();
    YIterator &operator++();
    void next();
};

class YCustomClass : public YObject {
    std::unordered_map<std::string, YObject*> dict_;

public:
    YCustomClass() : YObject { false, true } {};
    YCustomClass(const std::unordered_map<std::string, YObject*> &dict);
    void add(const std::string &key, YObject *val);
    YIterator *iter();
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

    YIterator *iter();

    std::deque<YObject> &value();

    YObject &operator[](size_t index);

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

    YObject &operator[](size_t index);

    YIterator *iter();

    size_t size() const;
    size_t hash();
};

class YDictObject : public YObject {
    std::map<YObject*, int> dict_;

public:
    YDictObject() : YObject { false, true } {};
    /*
    void add(const YObject &key, int value);
    
    YObject &get(const YObject &key);
    YDictObject(const YTupleObject &other);
    YDictObject &operator=(const YTupleObject &other);

    const YObject &operator[](size_t index) const;

    YIterator *begin();
    YIterator *end();

    size_t size() const;
    */
};

}
}
