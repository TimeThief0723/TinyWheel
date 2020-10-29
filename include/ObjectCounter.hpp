#pragma once
#include<atomic>
using namespace std;

namespace TinyWheel{

template<class T>
class ObjectCounter{
public:
    ObjectCounter(){
        count()++;
    }

    ~ObjectCounter(){
        count()--;
    }

    static atomic<int> &count(){
        static atomic<int> count(0);
        return count;
    }
};

}//namespace 