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

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}