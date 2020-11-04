#include <gtest/gtest.h>
#include <thread>
#include "ThreadPool.hpp"
#include "DummyObject.h"

using namespace std;
using namespace TinyWheel;

int func_sum(int64_t *data, size_t len, int64_t *ret){
    *ret = 0;
    for(int i = 0; i < len; i++){
        *ret += data[i];
    }
    return 0;
}

TEST(ThreadPool, sum)
{
    vector<int64_t> v(1000000);
    ThreadPool pool(100, 1000);

    int64_t sum = 0;
    for(size_t i = 0; i < v.size(); i++){
        v[i] = rand() % 1000000;
        sum += v[i];
    }

    int64_t sum2 = 0;

    auto single_sum = pool.enqueue(bind(func_sum, v.data(), v.size(), &sum2));
    single_sum.wait();
    ASSERT_EQ(sum2, sum);

    auto test_by_parallel_num = [&](int parallel_num){
        vector<int64_t> ret(parallel_num);
        pool.parallel_run<int64_t, int64_t>(func_sum, v.data(), v.size(), ret.data(), ret.size());
        int64_t result;
        func_sum(ret.data(), ret.size(), &result);
        ASSERT_EQ(result, sum);
    };

    vector<thread> threads;
    threads.emplace_back(test_by_parallel_num, 1);
    threads.emplace_back(test_by_parallel_num, 2);
    threads.emplace_back(test_by_parallel_num, 3);
    threads.emplace_back(test_by_parallel_num, 100);
    threads.emplace_back(test_by_parallel_num, 1000);
    threads.emplace_back(test_by_parallel_num, 10000);
    threads.emplace_back(test_by_parallel_num, 789);
    threads.emplace_back(test_by_parallel_num, 67);
    threads.emplace_back(test_by_parallel_num, 7);

    for(auto &t : threads){
        t.join();
    }
    
}

TEST(ThreadPool, early_stop)
{
    int v = 125;
    {
        ThreadPool pool(2, 100);
        pool.enqueue([&](){
            usleep(10000);
            v = 0;
            return 0;
        });
    }
    ASSERT_EQ(v, 125);
}