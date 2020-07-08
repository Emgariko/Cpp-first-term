#ifndef BIG_INTEGER_STORAGE
#define BIG_INTEGER_STORAGE

#include <cstddef>
#include <algorithm>
#include "buffer.h"

struct storage{
    //static const size_t STATIC_BUFFER_SIZE = 3;
    static const size_t STATIC_BUFFER_SIZE = sizeof(buffer*) / sizeof(uint32_t) + 2;
    storage();
    storage(const storage& rhs);
    ~storage();
    storage(size_t new_size, uint32_t x);
    storage& operator=(const storage& rhs);
    uint32_t& operator[](size_t index);
    uint32_t const& operator[](size_t index) const;
    void push_back(uint32_t x);
    void unshare();
    size_t size() const;
    uint32_t& back();
    void pop_back();

    void do_unshare();
    bool equals(const storage& a) const;
    friend bool operator!=(storage const& a, storage const& b);
    friend bool operator==(storage const& a, storage const& b);
    bool empty() const;
    void reverse();
private:
    bool small;
    size_t size_;
    union {
        buffer* dynamic_buffer;                     // big_object
        uint32_t static_buffer[STATIC_BUFFER_SIZE]; // small_object
    };
    void to_small();
    void to_big(uint32_t x);
};

#endif
