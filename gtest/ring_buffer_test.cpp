#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <utility>
#include <atomic>
#include <deque>
#include "ring_buffer.hpp"
#include "dummy_object.h"

using namespace std;
using namespace tiny_wheel;

TEST(RingBuffer, grammar)
{
    RingBuffer<OjbectOnlyMove> b1(10);
    OjbectOnlyMove a1,a2;
    b1.push(move(a1));
    b1.pop(a2);

    RingBuffer<OjbectOnlyCopy> b2(10);
    OjbectOnlyCopy b;
    b2.push(b);
    ASSERT_EQ(1,1);
}

TEST(RingBuffer, corner)
{
    RingBuffer<int> a(0);
    ASSERT_EQ(true, a.full());
    ASSERT_EQ(true, a.empty());

    RingBuffer<int> b(1);

    ASSERT_EQ(false, b.full());
    ASSERT_EQ(true, b.empty());

    b.push(12345);

    ASSERT_EQ(true, b.full());
    ASSERT_EQ(false, b.empty());
    int t;
    b.pop(t);

    ASSERT_EQ(false, b.full());
    ASSERT_EQ(true, b.empty());
    ASSERT_EQ(12345, t);

}

TEST(RingBuffer, random)
{
    for(int i = 0; i < 10; i++){
        int num = rand() % 10000;
        RingBuffer<int> b(num);
        deque<int> q;
        for(int i = 0; i < num; i++){
            b.push(i);
            q.push_back(i);
            if(i % 5 == 0){
                int v;
                b.pop(v);
                ASSERT_EQ(v , q.front());
                q.pop_front();
            }
        }
        while(!b.empty() && !q.empty()){
            int v;
            b.pop(v);
            ASSERT_EQ(v , q.front());
            q.pop_front();
        }
        ASSERT_EQ(b.empty(), q.empty());
    }
}
