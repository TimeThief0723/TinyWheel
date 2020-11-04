/* 
	github: https://github.com/TimeThief0723/TinyWheel
*/

#pragma once
#include <iostream>
#include <utility>
#include <vector>
using namespace std;

namespace TinyWheel{

template<class T>
class RingBuffer{
public:
    RingBuffer(size_t capacity):capacity_(capacity+1), data_(capacity_){ }

    template<class TypeWithRef>
    void push(TypeWithRef &&v){
        data_[end_] = forward<TypeWithRef>(v);
        end_ = (end_ + 1) % capacity_;
    }

    void pop(T &v){
        v = move(data_[start_]);
        start_ = (start_ + 1) % capacity_;
    }

    bool full(){
        return start_ == ((end_ + 1) % capacity_);
    }

    bool empty(){
        return start_ == end_;
    }

private:
    size_t start_ = 0;
    size_t end_ = 0;
    size_t capacity_ = 0;
    vector<T> data_;
};



};