#pragma once

#include <queue>
#include <mutex>
#include <optional>

template<typename T>
class SafeQueue
{
private:
    std::queue<T> queue;
    std::mutex mtx;
public:
    void push(T value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(value);
    }

    bool try_pop(T& out_value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        out_value = queue.front();
        queue.pop();
        return true;
    }
};