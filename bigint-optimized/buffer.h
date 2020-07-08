#ifndef BIG_INTEGER_BUFFER
#define BIG_INTEGER_BUFFER

#include <vector>
#include <cstdint>
#include <cassert>

struct buffer {
    size_t refs;
    buffer();
//    buffer(std::vector<uint32_t>& data);
    buffer(size_t size);
    buffer(std::vector<uint32_t>& data);
    buffer(size_t size, size_t value);
    uint32_t& operator[](size_t index);
    const uint32_t& operator[](size_t index) const;

    ~buffer();

    void link();
    void push_back(uint32_t x);
    void pop_back();
    void cut();
    std::vector<uint32_t>& data();
    uint32_t& back();
private:
    std::vector<uint32_t> data_;
};

#endif //BIGINT__BUFFER_H_