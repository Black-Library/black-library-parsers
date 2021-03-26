/**
 * ParserManager.h
 * 
 * Thread safe blocking queue
 */

#ifndef __BLOCKING_QUEUE_H__
#define __BLOCKING_QUEUE_H__

#include <functional>
#include <mutex>
#include <queue>

template <typename T>
class BlockingQueue {
private:
    std::mutex mutex_;
    std::queue<T> queue_;
public:
    T pop() {
        mutex_.lock();
        T value = queue_.front();
        queue_.pop();
        mutex_.unlock();
        return value;
    }

    void push(T value) {
        mutex_.lock();
        queue_.push(value);
        mutex_.unlock();
    }

    size_t size()
    {
        size_t size;
        mutex_.lock();
        size = queue_.size();
        mutex_.unlock();
        return size;
    }

    bool empty() {
        mutex_.lock();
        bool check = queue_.empty();
        mutex_.unlock();
        return check;
    }
};

#endif
