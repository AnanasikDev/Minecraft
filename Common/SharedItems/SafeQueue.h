#pragma once

#include <queue>
#include <deque>
#include <mutex>
#include <optional>

template<typename T>
class SafeQueue
{
private:
    std::deque<T> queue;
    std::mutex mtx;
public:
    void pushBack(T value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push_back(value);
    }

    void pushFront(T value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push_front(value);
    }

    bool tryPop(T& out_value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        out_value = queue.front();
        queue.pop_front();
        return true;
    }

    bool isEmpty()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }

    T& front()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.front();
    }
};
