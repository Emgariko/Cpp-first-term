#include <storage.h>

storage::storage() : small(true), size_(0) {}

storage::storage(size_t new_size, uint32_t x) {
    if (new_size <= STATIC_BUFFER_SIZE) {
        small = true;
        std::fill(static_buffer, static_buffer + new_size, x);
    } else {
        small = false;
        dynamic_buffer = new buffer(new_size, x);
    }
    size_= new_size;
}


storage::storage(const storage &rhs) {
    small = rhs.small;
    size_ = rhs.size_;
    if (small) {
        std::copy(rhs.static_buffer, rhs.static_buffer + size_, static_buffer);
    } else {
        dynamic_buffer = rhs.dynamic_buffer;
        dynamic_buffer->link();
    }
}

storage& storage::operator=(const storage& rhs) {
    if (this == &rhs) { return *this; }
    destruct();
    small = rhs.small;
    size_ = rhs.size_;
    if (small) {
        std::copy(rhs.static_buffer, rhs.static_buffer + size_, static_buffer);
    } else {
        dynamic_buffer = rhs.dynamic_buffer;
        dynamic_buffer->link();
    }
    return *this;
}

void storage::push_back(uint32_t x) {
    if (small && size_ < STATIC_BUFFER_SIZE) {
        static_buffer[size_++] = x;
    } else if (small && size_ == STATIC_BUFFER_SIZE) {
        to_big(x);
        size_++;
    } else if (!small) {
        do_unshare();
        dynamic_buffer->push_back(x);
        size_++;
    }
}

void storage::pop_back() {
    if (small) {
        size_--;
    } else {
        do_unshare();
        dynamic_buffer->pop_back();
        size_--;
    }
}

const uint32_t& storage::operator[](size_t index) const {
    return (small ? static_buffer[index] : (*dynamic_buffer)[index]);
}

uint32_t& storage::operator[](size_t index) {
    if (small) {
        return static_buffer[index];
    } else {
        do_unshare();
        return (*dynamic_buffer)[index];
    }
}

void storage::unshare() {
    assert(dynamic_buffer->refs > 1);
    buffer *data = new buffer(dynamic_buffer->data());
    dynamic_buffer->cut();
    dynamic_buffer = data;
}

size_t storage::size() const {
    return size_;
}

uint32_t& storage::back() {
    if (!small) {
        do_unshare();
    }
    return (small ? static_buffer[size_ - 1] : dynamic_buffer->back());
}

void storage::to_big(uint32_t x) {
    assert(size_ == STATIC_BUFFER_SIZE);
    buffer* new_buffer = new buffer();
    for (size_t i = 0; i < size_; i++) {
        new_buffer->push_back(static_buffer[i]);
    }
    new_buffer->push_back(x);
    dynamic_buffer = new_buffer;
    small = false;
}

bool operator!=(const storage &a, const storage &b) {
    return !(a == b);
}

bool operator==(const storage &a, const storage &b) {
    bool res = (a.size_ == b.size_);
    if (res) {
        for (size_t i = 0; i < a.size_; i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
    }
    return res;
}

bool storage::empty() const {
    return size_ == 0;
}

void storage::reverse() {
    if (small) {
        std::reverse(static_buffer, static_buffer + size_);
    } else {
        do_unshare();
        std::reverse(dynamic_buffer->data().begin(), dynamic_buffer->data().end());
    }
}

void storage::do_unshare() {
    if (dynamic_buffer->refs > 1) {
        unshare();
    }
}

storage::~storage() {
    destruct();
}

void storage::destruct() {
    if (!small) {
        if (dynamic_buffer->refs == 1) {
            delete dynamic_buffer;
        } else {
            dynamic_buffer->cut();
        }
    }
}
