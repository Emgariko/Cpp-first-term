#include <storage.h>

storage::storage() : small(true), size_(0) {
    std::fill(static_buffer, static_buffer + STATIC_BUFFER_SIZE, 0u);
}

storage::storage(size_t new_size, uint32_t x) {
    if (new_size <= STATIC_BUFFER_SIZE) {
        small = true;
        std::fill(static_buffer, static_buffer + STATIC_BUFFER_SIZE, 0u);
        for (size_t i = 0; i < new_size; i++) {
            static_buffer[i] = x;
        }
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
        std::fill(static_buffer, static_buffer + STATIC_BUFFER_SIZE, 0u);
        for (size_t i = 0; i < size_; i++) {
            static_buffer[i] = rhs.static_buffer[i];
        }
    } else {
        dynamic_buffer = rhs.dynamic_buffer;
        dynamic_buffer->link();
    }
}

bool storage::equals(const storage& rhs) const {
    bool res = rhs.size_ == size_ && rhs.small == small;
    if (res) {
        if (small) {
            for (size_t i = 0; i < size_; i++) {
                if (static_buffer[i] != rhs.static_buffer[i]) {
                    res = false;
                    break;
                }
            }
        } else {
            return dynamic_buffer == rhs.dynamic_buffer;
        }
    }
    return res;
}

storage& storage::operator=(const storage& rhs) {
    if (equals(rhs)) { return *this; }
    if (!small) {
        dynamic_buffer->cut();
        if (dynamic_buffer->refs == 0) {
            delete dynamic_buffer;
        }
    }
    small = rhs.small;
    size_ = rhs.size_;
    if (small) {
        std::fill(static_buffer, static_buffer + STATIC_BUFFER_SIZE, 0u);
        for (size_t i = 0; i < size_; i++) {
            static_buffer[i] = rhs.static_buffer[i];
        }
    } else {
        dynamic_buffer = rhs.dynamic_buffer;
        dynamic_buffer->link();
    }
    return *this;
}

void storage::push_back(uint32_t x) {
    if (size_ < STATIC_BUFFER_SIZE) {
        static_buffer[size_++] = x;
    } else if (size_ == STATIC_BUFFER_SIZE) {
        size_++;
        to_big(x);
    } else {
        size_++;
        do_unshare();
        dynamic_buffer->push_back(x);
    }
}

void storage::pop_back() {
    if (size_ <= STATIC_BUFFER_SIZE) {
        size_--;
    } else {
        do_unshare();
        dynamic_buffer->pop_back();
        size_--;
        if (size_ == STATIC_BUFFER_SIZE) {
            to_small();
        }
    }
}

const uint32_t& storage::operator[](size_t index) const {
    return (size_ <= STATIC_BUFFER_SIZE ? static_buffer[index] : (*dynamic_buffer)[index]);
}

uint32_t& storage::operator[](size_t index) {
    if (size_ <= STATIC_BUFFER_SIZE) {
        return static_buffer[index];
    } else {
        do_unshare();
        return (*dynamic_buffer)[index];
    }
}

void storage::unshare() {
    buffer *data = new buffer(dynamic_buffer->data());
    dynamic_buffer->cut();
    assert(dynamic_buffer->refs > 0);
    dynamic_buffer = data;
}

size_t storage::size() const {
    return size_;
}

uint32_t& storage::back() {
    return (size_ <= STATIC_BUFFER_SIZE ? static_buffer[size_ - 1] : dynamic_buffer->back());
}

void storage::to_small() {
    assert(size_ == STATIC_BUFFER_SIZE);
    dynamic_buffer->cut();
    small = true;
    uint32_t copy[STATIC_BUFFER_SIZE];
    for (size_t i = 0; i < size_; i++) {
        copy[i] = (*dynamic_buffer)[i];
    }
    delete dynamic_buffer;
    std::copy(copy, copy + STATIC_BUFFER_SIZE, static_buffer);
}

void storage::to_big(uint32_t x) {
    assert(size_ == STATIC_BUFFER_SIZE + 1);
    uint32_t copy[STATIC_BUFFER_SIZE];
    std::copy(static_buffer, static_buffer + STATIC_BUFFER_SIZE, copy);
    dynamic_buffer = new buffer();
    for (size_t i = 0; i < size_ - 1; i++) {
        dynamic_buffer->push_back(copy[i]);
    }
    dynamic_buffer->push_back(x);
    small = false;
}

bool operator!=(const storage &a, const storage &b) {
    return !(a == b);
}

bool operator==(const storage &a, const storage &b) {
    bool res = (a.small == b.small) && (a.size_ == b.size_);
    if (res) {
        if (a.small) {
            for (size_t i = 0; i < a.size_; i++) {
                if (a.static_buffer[i] != b.static_buffer[i]) {
                    res = false;
                    break;
                }
            }
        } else {
            res &= a.dynamic_buffer->data() == b.dynamic_buffer->data();
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
        std::reverse(dynamic_buffer->data().begin(), dynamic_buffer->data().end());
    }
}

void storage::do_unshare() {
    if (dynamic_buffer->refs > 1) {
        unshare();
    }
}

storage::~storage() {
    if (!small) {
        if (dynamic_buffer->refs == 1) {
            delete dynamic_buffer;
        } else {
            dynamic_buffer->cut();
        }
    }
}



