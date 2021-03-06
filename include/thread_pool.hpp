/* 
	github: https://github.com/TimeThief0723/TinyWheel
*/

#pragma once
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "blocking_queue.hpp"

using namespace std;

namespace tiny_wheel {

class WorkGroup {
  public:
    WorkGroup() : count_(1) {}

    void inc() {
        count_++;
    }

    void des() {
        count_--;
        if (count_ == 0) {
            promise_.set_value(ret_);
            delete this;
        }
    }

    std::promise<int> &promise() {
        return promise_;
    }

    int &ret() {
        return ret_;
    }

  private:
    atomic<int> count_;
    int ret_ = 0;
    std::promise<int> promise_;
};

class WorkItem {
  public:
    WorkItem() {}

    WorkItem(function<int(void)> &&func, WorkGroup *wg = nullptr) {
        func_ = func;
        group_ = wg;
    }

    void run() {
        int ret = func_();
        if (group_ == nullptr) {
            promise_.set_value(ret);
        } else {
            group_->ret() &= ret;
            group_->des();
        }
    }

    future<int> get_future() {
        return promise_.get_future();
    }

  private:
    function<int(void)> func_;
    std::promise<int> promise_;
    WorkGroup *group_;
};

class ThreadPool {
  public:
    ThreadPool(size_t thread_size, size_t queue_size = 1024) : queue_size_(queue_size), queue_(queue_size_) {
        for (int i = 0; i < thread_size; i++) {
            threads_.emplace_back(&ThreadPool::loop, this);
        }
    }

    ~ThreadPool() {
        queue_.destory();
        for (auto &t : threads_) {
            t.join();
        }
    }

    static WorkGroup *make_work_group() {
        return new WorkGroup();
    }

    static future<int> get_group_future(WorkGroup *group) {
        future<int> f = group->promise().get_future();
        group->des();
        return f;
    }

    void enqueue(std::function<int(void)> &&func, WorkGroup *group) {
        group->inc();
        WorkItem wi(move(func), group);
        queue_.push(move(wi));
    }

    future<int> enqueue(std::function<int(void)> &&func) {
        WorkItem wi(move(func));
        future<int> ret = wi.get_future();
        queue_.push(move(wi));
        return ret;
    }

    template <class INPUT_TYPE, class OUTPUT_TYPE>
    void parallel_run(std::function<int(INPUT_TYPE *, size_t, OUTPUT_TYPE *)> func, INPUT_TYPE *input, size_t input_len, OUTPUT_TYPE *output, size_t output_len) {
        size_t batch_size = (input_len + output_len - 1) / output_len;
        size_t offset = 0;
        auto work_group = make_work_group();
        for (int i = 0; i < output_len; i++) {
            size_t l = min(input_len - offset, batch_size);
            enqueue(bind(func, input + offset, l, output++), work_group);
            offset += l;
        }
        auto f = get_group_future(work_group);
        f.wait();
    }

  private:
    void loop() {
        while (!queue_.destoryed()) {
            WorkItem wi;
            if (!queue_.pop(wi)) {
                continue;
            }
            wi.run();
        }
    }

    size_t queue_size_;
    BlockingQueue<WorkItem> queue_;
    vector<thread> threads_;
};

}  // namespace tiny_wheel