//
// Created by theoking on 3/10/2020.
//

#include <vector>

#ifndef BENDER_BASE_UTILS_SRC_CIRCULAR_BUFFER_H_
#define BENDER_BASE_UTILS_SRC_CIRCULAR_BUFFER_H_

template <class T>
class CircularBuffer {
public:
    CircularBuffer(size_t size);
    bool Empty() const;
    std::size_t Size();
    std::size_t Capacity();
    void PushBack(const T &item);
    void PopFront();
    T& operator[](size_t pos);
    T& Front();
    T& Back();

private:
    std::vector<T> buffer_;
    size_t max_size_;
    size_t head_;
    size_t tail_;
    size_t content_size_;

    void IncrementHead();
    void IncrementTail();
};



template <class T>
CircularBuffer<T>::CircularBuffer(std::size_t size) {
    buffer_.resize(size);
    max_size_ = size;
    head_ = 1;
    tail_ = 0;
    content_size_ = 0;
}

template <class T>
bool CircularBuffer<T>::Empty() const {
    return content_size_ == 0;
}

template <class T>
size_t CircularBuffer<T>::Size() {
    return content_size_;
}

template <class T>
size_t CircularBuffer<T>::Capacity() {
    return max_size_;
}

template <class T>
void CircularBuffer<T>::IncrementTail() {
    ++tail_;
    ++content_size_;
    if (tail_ == max_size_) tail_ = 0;
}

template <class T>
void CircularBuffer<T>::IncrementHead(){
    if (!Empty()){
        ++head_;
        --content_size_;
        if (head_ == max_size_) head_ = 0;
    }
}

template <class T>
void CircularBuffer<T>::PushBack(const T &item) {
    IncrementTail();
    if (content_size_ > max_size_)
        IncrementHead();
    buffer_[tail_] = item;
}

template <class T>
void CircularBuffer<T>::PopFront() {
    IncrementHead();
}

template <class T>
T& CircularBuffer<T>::operator[](size_t pos) {
    return buffer_[(pos + head_) % max_size_];
}

template <class T>
T& CircularBuffer<T>::Front() {
    return buffer_[head_];
}

template <class T>
T& CircularBuffer<T>::Back() {
    return buffer_[tail_];
}

#endif