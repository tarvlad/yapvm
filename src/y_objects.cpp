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