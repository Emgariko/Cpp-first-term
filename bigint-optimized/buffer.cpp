#include "buffer.h"

buffer::buffer(size_t size) : data_(size), refs(1) {}

buffer::buffer(std::vector<uint32_t> &data) : refs(1), data_(data) {}

buffer::buffer(size_t size, size_t value) : refs(1), data_(size, value) {}

void buffer::link() {
    refs++;
}

void buffer::cut() {
    refs--;
}

void buffer::push_back(uint32_t x) {
    data_.push_back(x);
}

uint32_t &buffer::operator[](size_t index) {
    return data_[index];
}

const uint32_t &buffer::operator[](size_t index) const {
    return data_[index];
}

std::vector<uint32_t>& buffer::data() {
    return data_;
}

uint32_t& buffer::back() {
    return data_.back();
}

void buffer::pop_back() {
    data_.pop_back();
}

buffer::~buffer() = default;

buffer::buffer() : refs(1), data_() {}

