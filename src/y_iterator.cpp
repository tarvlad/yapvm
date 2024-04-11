#include "y_iterator.h"

namespace yapvm {
YIteratorList::YIteratorList(std::deque<YObject> &value) : iterator_{value} {}

YIterator* YIteratorList::next() {
	iterator_++;
	return this;
}

YObject &YIteratorList::operator*() {
	return *(iterator_);
}

YIterator &YIteratorList::operator++() {
	iterator_++;
	return *this;
}

}
