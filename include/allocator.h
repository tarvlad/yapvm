#pragma once


namespace yapvm::memory {

class Allocator {

public:
    Allocator();

    virtual ~Allocator() = 0;

    virtual void *allocate(size_t num_bytes) = 0;
    virtual void deallocate(void *data) = 0;
};

}