//
// Created by alaeddine on 10/29/17.
//

#ifndef FLOWOVER_SYNC_QUEUE_H
#define FLOWOVER_SYNC_QUEUE_H

#include "../includes.h"


template<typename T>
class sync_queue {
public:

    T pop() {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        auto val = queue_.front();
        queue_.pop();
        return val;
    }

    void pop(T &item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop();
    }

    void push(const T &item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }
    void clear()
    {
        std::queue<T> empty;
        std::swap( queue_, empty );
    }
    sync_queue() = default;

    sync_queue(const sync_queue &) = delete;            // disable copying
    sync_queue &operator=(const sync_queue &) = delete; // disable assignment

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


#endif //FLOWOVER_SYNC_QUEUE_H
