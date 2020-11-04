/*
        github: https://github.com/TimeThief0723/TinyWheel
*/

#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

using namespace std;

namespace tiny_wheel {

template <class T>
class BlockingQueue {
  public:
    BlockingQueue(size_t max_size = 1024) : max_size_(max_size) {}

    template <class TypeWithRef>
    bool push(TypeWithRef &&value) {
        unique_lock<mutex> lock(mutex_);
        cv_.wait(lock,
                 [&]() { return queue_.size() < max_size_ || destoryed_; });
        if (destoryed_) {
            return false;
        }
        queue_.emplace_back(std::forward<TypeWithRef>(value));
        cv_.notify_one();
        return true;
    }

    template <class TypeWithRef>
    bool try_push(TypeWithRef &&value) {
        unique_lock<mutex> lock(mutex_);
        if (queue_.size() >= max_size_) {
            return false;
        }
        queue_.emplace_back(std::forward<TypeWithRef>(value));
        cv_.notify_one();
        return true;
    }

    bool pop(T &ret) {
        unique_lock<mutex> lock(mutex_);
        cv_.wait(lock, [&]() { return queue_.size() > 0 || destoryed_; });
        if (destoryed_) {
            return false;
        }
        ret = move(queue_.front());
        queue_.pop_front();
        cv_.notify_one();
        return true;
    }

    bool try_pop(T &ret) {
        unique_lock<mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        ret = move(queue_.front());
        queue_.pop_front();
        cv_.notify_one();
        return true;
    }

    size_t size() {
        unique_lock<mutex> lock(mutex_);
        return queue_.size();
    }

    bool destoryed() { return destoryed_; }

    void destory() {
        destoryed_ = true;
        cv_.notify_all();
    }

  private:
    size_t max_size_;
    mutex mutex_;
    deque<T> queue_;
    condition_variable cv_;
    volatile bool destoryed_ = false;
};

}  // namespace tiny_wheel