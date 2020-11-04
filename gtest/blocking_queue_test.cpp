#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <utility>
#include <atomic>
#include "blocking_queue.hpp"
#include "dummy_object.h"

using namespace std;
using namespace TinyWheel;

TEST(BlockingQueue, grammar)
{
    BlockingQueue<OjbectOnlyMove> q1;
    OjbectOnlyMove a1,a2;
    q1.push(move(a1));
    q1.try_push(move(a2));
    q1.pop(a2);
    q1.try_pop(a1);

    BlockingQueue<OjbectOnlyCopy> q2;
    OjbectOnlyCopy b;
    q2.push(b);
    q2.try_push(b);
    ASSERT_EQ(1,1);
}

TEST(BlockingQueue, sum)
{
    BlockingQueue<uint64_t> queue;
    vector<thread> producer;
    vector<thread> consumer;
    atomic<uint64_t> producer_sum(0);
    atomic<int> producer_count(0);
    atomic<uint64_t> consumer_sum(0);
    atomic<int> consumer_count(0);
    for(int i = 0; i < 100; i++){
        producer.emplace_back([&]() mutable {
            for(int i = 0; i < 10000; i++){
                uint64_t v = rand()%1000;
                producer_sum += v;
                producer_count++;
                queue.push(v);
            }
        });
    }

    for(int i = 0; i < 100; i++){
        consumer.emplace_back([&]() mutable {
            while(true){
                uint64_t v;
                queue.pop(v);
                if(queue.destoryed()){
                    break;
                }
                consumer_sum += v;
                consumer_count++;
            }
        });
    }

    //wait all producer finish
    for(auto &p : producer){
        p.join();
    }
    
    //wait all consumer finish
    while(queue.size() != 0);

    queue.destory();

    for(auto &p : consumer){
        p.join();
    }

    ASSERT_EQ(producer_count, consumer_count);
    ASSERT_EQ(producer_sum, consumer_sum);
}