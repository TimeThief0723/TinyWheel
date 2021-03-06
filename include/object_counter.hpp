/*
        github: https://github.com/TimeThief0723/TinyWheel
*/

#pragma once
#include <atomic>
using namespace std;

namespace tiny_wheel {

template <class T>
class ObjectCounter {
  public:
    ObjectCounter() { count()++; }

    ~ObjectCounter() { count()--; }

    static atomic<int> &count() {
        static atomic<int> count(0);
        return count;
    }
};

}  // namespace tiny_wheel