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
    decltype(auto) begin()
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        map_.begin();
    }

    decltype(auto) emplace(Key key, Value value)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        return map_.emplace(key, value);
    }

    decltype(auto) erase(Key key)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        return map_.erase(key);
    }

    decltype(auto) end()
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        map_.end();
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

    decltype(auto) find(Key key)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        return map_.find(key);
    }

    bool key_exists(Key key)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        if (map_.find(key) == map_.end())
            return false;

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
