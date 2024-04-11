#include "y_objects.h"
#include "utils.h"
#include <stdexcept>
#include <vector>


using namespace yapvm::yobjects;

namespace yapvm {
YObject::YObject(bool is_hashable, bool is_iterable) : is_hashable_{ is_hashable }, is_iterable_{ is_iterable } {}

bool YObject::is_hashable() const {
    return is_hashable_;
}

bool &YObject::is_marked() {
    return is_marked_;
}

YCustomClasses::YCustomClasses(const std::map<std::string, YObject>& dict) : YObject{false, true}, dict_{ dict } {}

// YIterator* YCustomClasses::begin() {
//     return new YIteratorList(dict_.begin());
// }

// YIterator* YCustomClasses::end() {
//     return new YIteratorList(dict_.end());

// YIterator *YCustomClasses::iter() {
//     // TODO
// }


YIteratorList::YIteratorList(const std::deque<YObject>::iterator &value) : iterator_{value} {}

YIterator* YIteratorList::next() {
	iterator_++;
	return this;
}

YBoolObject::YBoolObject(bool value) : YPrimitiveObject{true, false}, value_ { value } {}

YIterator* YBoolObject::begin() {
    return nullptr;
}

YIterator* YBoolObject::end() {
    return nullptr;
}

bool YBoolObject::value() const {
    return value_;
}


YStringObject::YStringObject(const std::string &value) : YPrimitiveObject{true, false}, value_{ value } {}

YIterator* YStringObject::begin() {
    return nullptr;
}

YIterator* YStringObject::end() {
    return nullptr;
}

const std::string &YStringObject::value() const {
    return value_;
}


YFloatObject::YFloatObject(double value) : YPrimitiveObject{true, false}, value_{ value } {}

YIterator* YFloatObject::begin() {
    return nullptr;
}

YIterator* YFloatObject::end() {
    return nullptr;
}

double YFloatObject::value() const {
    return value_;
}


YIntObject::YIntObject(ssize_t value) : YPrimitiveObject{true, false}, value_{ value } {}

YIterator* YIntObject::begin() {
    return nullptr;
}

YIterator* YIntObject::end() {
    return nullptr;
}

ssize_t YIntObject::value() const {
    return value_;
}

YListObject::YListObject(const std::deque<YObject> &value) : YObject{ false, true },  list_ { value } {}


YListObject::YListObject(const std::vector<YObject> &value) : YObject{ false, true } {
    // TODO
};


YListObject::YListObject(const YListObject &value) : YObject{false, true}, list_{ value.list_ } {}


YListObject &YListObject::operator=(const YListObject &other) {
    if (&other == this) {
        return *this;
    }
    list_ = other.list_;
    return *this;
}


std::deque<YObject> &YListObject::value() {
    return list_;
}

// YIterator *YListObject::iter() {
//     return new YIteratorList{ value() };
// }


const YObject &YListObject::operator[](size_t index) const {
    if (index >= list_.size()) {
        throw std::range_error("IndexError: list index out of range");
    }
    return list_.at(index);
}


YListObject YListObject::operator+(const YListObject &other) {
    YListObject tmp{ list_ };
    // TODO
    for (YObject element : other.list_) {
        tmp.list_.push_back(element);
    }
    return tmp;
}

YObject &YIteratorList::operator*() {
	return *(iterator_);
}

YIterator &YIteratorList::operator++() {
	iterator_++;
	return *this;
}

YIterator* YListObject::begin() {
    return new YIteratorList(list_.begin());
}

YIterator* YListObject::end() {
    return new YIteratorList(list_.end());
}


void YListObject::append(const YObject &val) {
    list_.push_front(val);
}


size_t YListObject::size() const {
    return list_.size();
}

// TUPLE
// YTupleObject::YTupleObject(size_t size) : YObject{true, true}, tuple_{ size } {}


// YTupleObject::YTupleObject(const std::vector<YObject> &value) : YTupleObject(value.size()) {
//     for (size_t i = 0; i < value.size(); i++) {
//         tuple_[i] = value[i];
//     }
// }


// YTupleObject::YTupleObject(const YTupleObject &other) : YObject{true, true}, tuple_{ other.tuple_ } {}


// YTupleObject &YTupleObject::operator=(const YTupleObject &other) {
//     if (&other == this) {
//         return *this;
//     }
//     tuple_ = other.tuple_;
//     return *this;
// }


// const YObject &YTupleObject::operator[](size_t index) const {
//     if (index >= tuple_.size()) {
//         throw std::range_error("IndexError: list index out of range");
//     }
//     return tuple_[index];
// }


// size_t YTupleObject::size() const {
//     return tuple_.size();
// }
// TUPLE
}