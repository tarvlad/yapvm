#include "y_objects.h"
#include "utils.h"
#include <stdexcept>
#include <sys/types.h>
#include <vector>


using namespace yapvm::yobjects;

namespace yapvm {
YObject::YObject(bool is_hashable, bool is_iterable) : is_hashable_{ is_hashable }, is_iterable_{ is_iterable } {}

bool YObject::is_hashable() const {
    return is_hashable_;
}

bool YObject::is_marked() {
    return is_marked_;
}

void YObject::mark() {
    is_marked_ = true;
}

YCustomClass::YCustomClass(const std::unordered_map<std::string, YObject*> &dict) : YObject{false, true}, dict_{ dict } {}

void YCustomClass::add(const std::string &key, YObject *val) {
    dict_[key] = val;
}
YIterator *YCustomClass::iter() {
    return new YIteratorCustomClass {dict_.begin(), dict_.end()};
}
// YIterator* YCustomClasses::begin() {
//     return new YIteratorList(dict_.begin());
// }

// YIterator* YCustomClasses::end() {
//     return new YIteratorList(dict_.end());

// YIterator *YCustomClasses::iter() {
//     // TODO
// }


size_t YHash::operator()(const YObject &val) {
    return val.hash();
}


YBoolObject::YBoolObject(bool value) : YPrimitiveObject{true, false}, value_ { value } {}


bool YBoolObject::value() const {
    return value_;
}


YStringObject::YStringObject(const std::string &value) : YPrimitiveObject{true, false}, value_{ value } {}


const std::string &YStringObject::value() const {
    return value_;
}


YFloatObject::YFloatObject(double value) : YPrimitiveObject{true, false}, value_{ value } {}


double YFloatObject::value() const {
    return value_;
}


YIntObject::YIntObject(ssize_t value) : YPrimitiveObject{true, false}, value_{ value } {}


ssize_t YIntObject::value() const {
    return value_;
}

YListObject::YListObject(const std::deque<YObject> &value) : YObject{ false, true },  list_ { value } {}


YListObject::YListObject(const std::vector<YObject> &value) : YObject{ false, true } {
    for (YObject elem : value) {
        list_.push_back(elem);        
    }
};


YListObject::YListObject(const YListObject &value) : YObject{false, true}, list_{ value.list_ } {}


YListObject &YListObject::operator=(const YListObject &other) {
    if (&other == this) {
        return *this;
    }
    list_ = other.list_;
    return *this;
}

YIterator *YListObject::iter() {
    return new YIteratorList { list_.begin(), list_.end() };
}

std::deque<YObject> &YListObject::value() {
    return list_;
}


YObject &YListObject::operator[](size_t index) {
    if (index >= list_.size()) {
        throw std::range_error("IndexError: list index out of range");
    }
    return list_.at(index);
}


YListObject YListObject::operator+(const YListObject &other) {
    YListObject tmp{ list_ };
   
    for (YObject element : other.list_) {
        tmp.list_.push_back(element);
    }
    return tmp;
}

YIteratorList::YIteratorList(const std::deque<YObject>::iterator &begin, const std::deque<YObject>::iterator &end) : begin_{begin}, end_{end} {}

bool YIteratorList::has_next() {
    return begin_ < end_;
}

YObject &YIteratorList::operator*() {
    return *(begin_);
}

YIterator &YIteratorList::operator++() {
	++begin_;
	return *this;
}

void YIteratorList::next() {
    ++begin_;
}

void YListObject::append(const YObject &val) {
    list_.push_front(val);
}


size_t YListObject::size() const {
    return list_.size();
}


YTupleObject::YTupleObject(const std::vector<YObject> &value) : YObject { true, true }, tuple_ { value } {}


YTupleObject::YTupleObject(const YTupleObject &other) : YObject{ true, true }, tuple_{ other.tuple_ } {}


YTupleObject &YTupleObject::operator=(const YTupleObject &other) {
    if (&other == this) {
        return *this;
    }
    tuple_ = other.tuple_;
    return *this;
}


YObject &YTupleObject::operator[](size_t index) {
    if (index >= tuple_.size()) {
        throw std::range_error("IndexError: list index out of range");
    }
    return tuple_[index];
}

YIterator *YTupleObject::iter() {
    return new YIteratorTuple { tuple_.begin(), tuple_.end() };
}

size_t YTupleObject::size() const {
    return tuple_.size();
}


size_t YTupleObject::hash() {
    size_t h = tuple_[0].hash();
    for (size_t i = 1; i < tuple_.size(); i++) {
        h = combine_hashes(h, tuple_[i].hash());        
    }
    return h;        
}


}

/*
void YDictObject::add(const YObject &key, int value) {
    dict_.insert(key, value);
}
*/ 

/*
YObject &YDictObject::get(const YObject &key) {
    return dict_[key];
}
*/

YIteratorTuple::YIteratorTuple(const std::vector<YObject>::iterator &begin, const std::vector<YObject>::iterator &end) : begin_ {begin}, end_ {end}  {}

bool YIteratorTuple::has_next() {
    return begin_ < end_;
}

YObject &YIteratorTuple::operator*() {
    return *(begin_);
}

YIterator &YIteratorTuple::operator++() {
    ++begin_;
    return *this;
}

void YIteratorTuple::next() {
    ++begin_;
}

YIteratorCustomClass::YIteratorCustomClass(const std::unordered_map<std::string, YObject*>::iterator &begin, const std::unordered_map<std::string, YObject*>::iterator &end) : begin_ {begin}, end_ {end} {}

bool YIteratorCustomClass::has_next() {
    return begin_ != end_;
}

YObject &yapvm::yobjects::YIteratorCustomClass::operator*() {
    return *(*begin_).second;
}

YIterator &YIteratorCustomClass::operator++() {
    ++begin_;
    return *this;
}

void YIteratorCustomClass::next() {
    ++begin_;
}
