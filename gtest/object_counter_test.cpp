#include <gtest/gtest.h>
#include <vector>
#include "object_counter.hpp"

using namespace std;
using namespace tiny_wheel;

class TestA : public ObjectCounter<TestA> {
    int a;
};

class TestB : public ObjectCounter<TestB> {
    int b;
};

TEST(ObjectCounter, general){
    vector<TestA> va(10);
    vector<TestB> vb(20);

    TestA a;
    static TestA sa;

    ASSERT_EQ(ObjectCounter<TestA>::count(), va.size() + 2);
    ASSERT_EQ(ObjectCounter<TestB>::count(), vb.size());

    for(int i = 0; i < 5; i++){
        va.pop_back();
        vb.pop_back();
    }

    ASSERT_EQ(ObjectCounter<TestA>::count(), va.size() + 2);
    ASSERT_EQ(ObjectCounter<TestB>::count(), vb.size());

}