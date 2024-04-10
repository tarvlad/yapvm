#include <iterator>
#include "y_objects.h"

namespace yapvm {
namespace yobjects {

// Wrapper for std::iterator
class YIterator {
public:
	virtual YIterator* next() = 0;
	virtual YObject &operator*() = 0;
	virtual YIterator &operator++() = 0;
	virtual ~YIterator() = default;
};

class YIteratorList : public YIterator {
	std::deque<YObject>::iterator iterator_;
	std::deque<YObject>::iterator end_;

public: 
	YIteratorList(std::deque<YObject, std::allocator<YObject>> &value);
	YIterator *next();
	YObject &operator*();
	YIterator &operator++();
	~YIteratorList() = default;
};

}
}