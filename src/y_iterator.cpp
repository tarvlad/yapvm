#include "y_iterator.h"
namespace yapvm {
namespace yobjects {
	YIteratorList::YIteratorList(std::deque<YObject, std::allocator<YObject>> &value) : iterator_{ value.begin() }, end_{ value.end() } {}

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
}
