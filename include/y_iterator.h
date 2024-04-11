#include "y_objects.h"
#include <iterator>

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

public: 
	YIteratorList(std::deque<YObject>::iterator &value);
	std::deque<YObject>::iterator& iter();
	YIterator *start();
	YIterator *end();
	YIterator *next();
	YObject &operator*();
	YIterator &operator++();
	~YIteratorList() = default;
};

}
}