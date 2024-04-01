#include "y_objects.h"


using namespace yapvm::yobjects;


YBoolObject::YBoolObject(bool value) : value_{ value } {}


bool YBoolObject::value() const {
    return value_;
}


YStringObject::YStringObject(const std::string &value) : value_{ value } {}


const std::string &YStringObject::value() const {
    return value_;
}


YFloatObject::YFloatObject(double value) : value_{ value } {}


double YFloatObject::value() const {
    return value_;
}


YIntObject::YIntObject(ssize_t value) : value_{ value } {}


ssize_t YIntObject::value() const {
    return value_;
}

YListObject::YListObject(const std::deque<YObject> &value) : list_{ value } {}


YListObject::YListObject(const std::vector<YObject> &value) {
    for (YObject element : value) {
        list_.push_back(element);
    }
};


YListObject::YListObject(const YListObject &value) : list_{ value.list_ } {}


YListObject& YListObject::operator=(const YListObject &other) {
    if (&other == this) {
        return *this;
    }
    list_ = other.list_;
    return *this;
}


const std::deque<YObject> &YListObject::value() const {
    return list_;
}


const YObject &YListObject::operator[](size_t index) const {
    if (index > list_.size()) {
        throw std::range_error("IndexError: list index out of range");
    }
    return list_.at(index);
}


YListObject YListObject::operator+(const YListObject &other) {
    YListObject tmp {list_};
    for (YObject element : other.list_) {
        tmp.list_.push_back(element);
    }
    return tmp;
}


void YListObject::append(const YObject &val) {
    list_.push_front(val);
}


size_t YListObject::size() const {
    return list_.size();
}

//Broken Dict
/*
YDictObject::YDictObject(const YDictObject &value) : dict_{ value.dict_ } {}


YDictObject &YDictObject::operator=(const YDictObject &other) {
    dict_ = other.dict_;
}


void YDictObject::add(const YObject &key, const YObject &value) {

    dict_[key] = value;
}


const YObject &YDictObject::get(const YObject &key) const {
    if (dict_.find(key) == dict_.end()) {
        throw std::runtime_error("Key error");
    }
    return dict_.at(key);
}


const YObject &YDictObject::operator[](const YObject& key) const {
    return get(key);
}
*/


YTupleObject::YTupleObject(size_t size) : tuple_{ size } {}


YTupleObject::YTupleObject(const std::vector<YObject> &value) : YTupleObject(value.size())   {
    for (size_t i = 0; i < value.size(); i++) {
        tuple_[i] = value[i];
    }
}


YTupleObject::YTupleObject(const YTupleObject& other) : tuple_{ other.tuple_ } {}


YTupleObject &YTupleObject::operator=(const YTupleObject &other) {
    if (&other == this) {
        return *this;
    }
    tuple_ = other.tuple_;
    return *this;
}


const YObject &YTupleObject::operator[](size_t index) const {
    if (index > tuple_.size()) {
        throw std::range_error("IndexError: list index out of range");
    }
    return tuple_[index];
}


size_t YTupleObject::size() const {
    return tuple_.size();
}