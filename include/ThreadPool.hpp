/* 
	github: https://github.com/TimeThief0723/TinyWheel
*/

#pragma once
#include<condition_variable>
#include<mutex>
#include<queue>
#include<utility>
#include<thread>
#include<future>
#include<atomic>
#include "BlockingQueue.hpp"

using namespace std;

namespace TinyWheel{

class WorkGroup{
public:
	WorkGroup():count(1){}
	
	void inc(){
		count++;
	}

	void des(){
		count--;
		if(count == 0){
			promise.set_value(ret);
			delete this;
		}
		
	}

	atomic<int> count;
	int ret = 0;
	std::promise<int> promise;
};

class WorkItem{
public:
	WorkItem(){}

	WorkItem(function<int(void)> &&func, WorkGroup *wg = nullptr){
		func_ = func;
		group_ = wg;
	}

	void run(){
		int ret = func_();
		if(group_ == nullptr){
			promise_.set_value(ret);
		}
		else{
			group_->ret &= ret;
			group_->des();
		}
	}

	future<int> get_future(){
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
		for(int i = 0; i < thread_size; i++){
			threads_.emplace_back(&ThreadPool::loop, this);
		}
	}

	~ThreadPool(){
		queue_.destory();
		for(auto &t : threads_){
			t.join();
		}
	}

	static WorkGroup *make_work_group(){
		return new WorkGroup();
	}
	 
	static future<int> get_group_future(WorkGroup *group){
		future<int> f = group->promise.get_future();
		group->des();
		return f;
	}

	void enqueue(std::function<int(void)> &&func, WorkGroup *group){
		group->inc();
		WorkItem wi(move(func), group);
		queue_.push(move(wi));
    }

    future<int> enqueue(std::function<int(void)> &&func){
		WorkItem wi(move(func));
		future<int> ret = wi.get_future();
		queue_.push(move(wi));
        return ret;
    }

	void loop(){
		while(!queue_.destoryed()){
			WorkItem f;
			if(!queue_.pop(f)){
				continue;
			}
			f.run();
        }
	}

private:
	size_t queue_size_;
	BlockingQueue<WorkItem> queue_;
	vector<thread> threads_;
};

} //namespace 