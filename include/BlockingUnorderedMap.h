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
class BlockingUnorderedMap
{
private:
    std::mutex mutex_;
    std::unordered_map<Key, Value> map_;
public:
    // TODO: emplace actually returns something
    void emplace(Key key, Value value)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        map_.emplace(key, value);
    }

    // TODO: erase actually returns something
    void erase(Key key)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        map_.erase(key);
    }

    Value find(Key key)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        Value value = map_.find(key);
        return value;
    }

    bool find_and_replace(Key key, Value value)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        if (map_.find(key) == map_.end())
            return false;

        map_.erase(key);

        map_.emplace(key, value);

        return true;
    }

    size_t size()
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        size_t size = map_.size();
        return size;
    }

    bool empty()
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        bool check = map_.empty();
        return check;
    }
};

#endif
