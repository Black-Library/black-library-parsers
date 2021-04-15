/**
 * BlockingUnorderedMap.h
 * 
 * Thread safe blocking unordered map
 */

#ifndef __BLOCKING_UNORDERED_MAP_H__
#define __BLOCKING_UNORDERED_MAP_H__

#include <functional>
#include <mutex>
#include <unordered_map>

template <typename Key, typename Value>
class BlockingUnorderedMap {
private:
    std::mutex mutex_;
    std::unordered_map<Key, Value> map_;
public:
    // T pop() {
    //     const std::lock_guard<std::mutex> lock(mutex_);
    //     T value = queue_.front();
    //     queue_.pop();
    //     return value;
    // }

    // void push(T value) {
    //     const std::lock_guard<std::mutex> lock(mutex_);
    //     queue_.push(value);
    // }

    size_t size()
    {
        size_t size;
        const std::lock_guard<std::mutex> lock(mutex_);
        size = map_.size();
        return size;
    }

    bool empty() {
        const std::lock_guard<std::mutex> lock(mutex_);
        bool check = map_.empty();
        return check;
    }
};

#endif
