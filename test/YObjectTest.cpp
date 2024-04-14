#include <gtest/gtest.h>
#include "y_objects.h"
using namespace yapvm;
using namespace yobjects;

TEST(YListTest, ListCtr) {
    YListObject list {};    
    YBoolObject b {true};
    list.append(b);
}

TEST(YListTest, VectorCtr) {
    std::vector<YObject> vec = { YFloatObject {3.1415}, YIntObject { 42 }, YListObject {} };
    YListObject list {vec}; 
    EXPECT_EQ(true, list[0].is_hashable());
    EXPECT_EQ(true, list[1].is_hashable());
    EXPECT_EQ(false, list[2].is_hashable());
}

TEST(YListTest, Iter) {
    std::vector<YObject> vec = { YFloatObject {3.1415}, YIntObject { 42 }, YListObject {} };
    YListObject list {vec}; 
    YIterator* it = list.iter();
    int cnt = 0;
    while (it->has_next()) {
        cnt++;
        (**it).mark();
        it->next();
    }
    delete it;
    for (size_t i = 0; i < list.size(); i++) {
        EXPECT_EQ(true, list[i].is_marked());
    }
}


TEST(YTupleTest, VectorCtr) {
    std::vector<YObject> vec = { YFloatObject {3.1415}, YIntObject { 42 }, YListObject {} };
    YTupleObject list {vec}; 
    EXPECT_EQ(true, list[0].is_hashable());
    EXPECT_EQ(true, list[1].is_hashable());
    EXPECT_EQ(false, list[2].is_hashable());
}

TEST(YTupleTest, Iter) {
    std::vector<YObject> vec = { YFloatObject {3.1415}, YIntObject { 42 }, YListObject {} };
    YTupleObject tuple {vec}; 
    YIterator* it = tuple.iter();
    int cnt = 0;
    while (it->has_next()) {
        cnt++;
        (**it).mark();
        it->next();
    }
    delete it;
    for (size_t i = 0; i < tuple.size(); i++) {
        EXPECT_EQ(true, tuple[i].is_marked());
    }
}
int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}