#include <cstring>
#include <cstddef>
#include <cassert>
#include <algorithm>
#include <iostream>

#ifndef VECTOR_H
#define VECTOR_H

template <typename T>
struct vector
{
    typedef T* iterator;
    typedef T const* const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector<T>& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    //iterator insert(iterator pos, T const&); // O(N) weak
    iterator insert(const_iterator pos, T const&); // O(N) weak

    //iterator erase(iterator pos);           // O(N) weak
    iterator erase(const_iterator pos);     // O(N) weak

    //iterator erase(iterator first, iterator last); // O(N) weak
    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    void push_back_realloc(T const&);
    void new_buffer(size_t new_capacity);
    void static copy_all(T* dst, T const* src, size_t size);
    void static destruct_all(T const* src, size_t before);
private:
    T* data_;
    size_t size_;
    size_t capacity_;
};


template<typename T>
vector<T>::vector() {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
void vector<T>::destruct_all(const T* src, size_t before) {
    for (size_t i = before; i > 0; i--) {
        src[i - 1].~T();
    }
}
template<typename T>
void vector<T>::copy_all(T* dst, T const* src, size_t size) {
    size_t i = 0;
    try {
        for (; i < size; ++i) {
            new (dst + i) T(src[i]);
        }
    } catch (...) {
        destruct_all(dst, i);
        throw;
    }
}


template<typename T>
vector<T>::vector(vector<T> const& other) {
    size_ = 0;
    data_ = nullptr;
    capacity_ = 0;
    if (other.capacity_ == 0) {
        destruct_all(data_, size_);
        operator delete(data_);
        capacity_ = 0;
        size_ = 0;
        data_ = nullptr;
    } else {
        T *new_data = static_cast<T*>(operator new(other.capacity_ * sizeof(T)));
        try {
            copy_all(new_data, other.data_, other.size_);
        } catch (...) {
            operator delete(new_data);
            throw;
        }
        destruct_all(data_, size_);
        operator delete(data_);
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = new_data;
    };
}

template<typename T>
vector<T>& vector<T>::operator=(vector<T> const& other) {
    if (this == &other) {
        return *this;
    }
    vector<T> copy(other);
    swap(copy);
    return *this;
}

template<typename T>
vector<T>::~vector() {
    destruct_all(data_, size_);
    operator delete(data_);
}

template<typename T>
T& vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const& vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
T* vector<T>::data() {
    return data_;
}

template<typename T>
T const* vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T& vector<T>::front() {
    return *data_;
}

template<typename T>
T const& vector<T>::front() const {
    return *data_;
}

template<typename T>
T& vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const& vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
void vector<T>::push_back(const T & x) {
    if (size_ != capacity_) {
        new(data_ + size_) T(x);
        size_++;
    } else {
        push_back_realloc(x);
    }
}

template<typename T>
void vector<T>::push_back_realloc(const T &x) {
    vector <T> copy_vector(*this);
    copy_vector.reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    copy_vector.push_back(x);
    swap(copy_vector);
}

template<typename T>
void vector<T>::pop_back() {
    (data_ + size_ - 1)->~T();
    size_--;
}

template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::reserve(size_t new_cap) {
    if (capacity_ < new_cap) {
        new_buffer(new_cap);
    }
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size_ < capacity_) {
        new_buffer(size_);
    }
}

template<typename T>
void vector<T>::clear() {
    destruct_all(data_, size_);
    size_ = 0;
}

template<typename T>
void vector<T>::swap(vector &other) {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::end() {
    return (data_ + size_);
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return (data_);
}


template<typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return (data_ + size_);
}

template<typename T>
void vector<T>::new_buffer(size_t new_capacity) {
    assert(new_capacity >= size_);
    vector <T> copy_vector;
    if (new_capacity > 0) {
        copy_vector.data_ = static_cast<T*>(operator new(sizeof(T) * new_capacity));
        copy_all(copy_vector.data_, data_, size_);
        copy_vector.size_ = size_;
        copy_vector.capacity_ = new_capacity;
    }
    swap(copy_vector);
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T& x) {
    if (size_ == 0 && pos == begin()) {
        push_back(x);
        return data_;
    } else {
        ptrdiff_t pos_ = pos - data_;
        push_back(x);
        for (ptrdiff_t i = size_ - 1; i != pos_; i--) {
            std::swap(*(data_ + i), *(data_ + i - 1));
        }
        return (data_ + pos_);
    }
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    ptrdiff_t first_ = first - data_;
    ptrdiff_t last_ = last - data_ - 1;
    ptrdiff_t rest_size = data_ + size_ - last;
    ptrdiff_t len = last_ - first_ + 1;
    for (ptrdiff_t i = 0; i < rest_size; i++) {
        std::swap(data_[first_ + i], data_[first_ + len + i]);
    }
    for (ptrdiff_t i = 0; i < len; i++) {
        pop_back();
    }
    shrink_to_fit();
    return begin() + first_;
}


#endif // VECTOR_H
