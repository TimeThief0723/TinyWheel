#pragma once
#include<condition_variable>
#include<mutex>
#include<queue>
#include<utility>

using namespace std;

template<class T>
class BlockingQueue {
public:
	BlockingQueue(size_t max_size = 1024) : m_max_size(max_size) {}

	template<class TypeWithRef>
	void push(TypeWithRef &&value) {
		unique_lock<mutex> lock(m_mutex);
		m_cv.wait(lock, [&]() { return m_queue.size() < m_max_size || destoryed_; });
		if(destoryed_){
			return ;
		}
		m_queue.emplace_back(std::forward<TypeWithRef>(value));
		m_cv.notify_one();
	}

	template<class TypeWithRef>
    bool try_push(TypeWithRef &&value){
        unique_lock<mutex> lock(m_mutex);
        if( m_queue.size() >= m_max_size ){
            return false;
        }
        m_queue.emplace_back(std::forward<TypeWithRef>(value));
		m_cv.notify_one();
        return true;
    }

	void pop(T &ret) {
		unique_lock<mutex> lock(m_mutex);
		m_cv.wait(lock, [&]() { return m_queue.size() > 0 || destoryed_; });
		if(destoryed_){
			return ;
		}
		ret = move(m_queue.front());
		m_queue.pop_front();
		m_cv.notify_one();
	}

	bool try_pop(T &ret){
		unique_lock<mutex> lock(m_mutex);
		if(m_queue.empty()){
			return false;
		}
		ret = move(m_queue.front());
		m_queue.pop_front();
		m_cv.notify_one();
		return true;
	}

	size_t size(){
		unique_lock<mutex> lock(m_mutex);
		return m_queue.size();
	}

	bool destoryed(){
		return destoryed_;
	}

	void destory(){
		destoryed_ = true;
		m_cv.notify_all();
	}

private:
	size_t m_max_size;
	mutex m_mutex;
	deque<T> m_queue;
	condition_variable m_cv;
	volatile bool destoryed_ = false;
};