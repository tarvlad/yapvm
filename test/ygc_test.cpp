#include <gtest/gtest.h>
#include "y_objects.h"
#include "gc.h"

using namespace yapvm;
using namespace yobjects;
using namespace ygc;

//TEST(YGCTest, GCMarkAll) {//pizdec...
//    YFloatObject fo{3.14};
//    YIntObject io{23162};
//    YListObject lo{std::vector<YObject>{YIntObject{1}, YIntObject{2}, YIntObject{3}}};
//
//    std::vector<YObject*> hp_vec{std::vector<YObject*>{&fo, &io, &lo}};
//    std::map<std::string, YObject*> sp_map;
//    sp_map["fo"] = &fo;
//    sp_map["io"] = &io;
//    sp_map["lo"] = &lo;
//
//    YHeap hp{hp_vec};
//    YScope sp{sp_map};
//
//    YGC gc{&sp, &hp};
//    gc.mark();
//    EXPECT_EQ(true, fo.is_marked());
//    EXPECT_EQ(true, io.is_marked());
//    for (size_t i = 0; i < lo.size(); i++) {
//        EXPECT_EQ(true, lo[i].is_marked());
//    }
//}
//
//TEST(YGCTest, GCMarkPartial) {
//    YFloatObject fo{3.14};
//    YIntObject io{23162};
//    YListObject lo1{std::vector<YObject>{YIntObject{1}, YIntObject{2}, YIntObject{3}}};
//    YListObject lo2{std::vector<YObject>{YIntObject{4}, YIntObject{5}, YIntObject{6}}};
//
//    std::vector<YObject*> hp_vec{std::vector<YObject*>{&fo, &io, &lo1, &lo2}};
//    std::map<std::string, YObject*> sp_map;
//    sp_map["fo"] = &fo;
//    sp_map["io"] = &io;
//    // smth like l1 = [1, 2, 3] -> l1 = [4, 5, 6], so 1, 2, 3
//    // io stay unmarked and must be sweeped
//    sp_map["lo1"] = &lo2;
//
//    YHeap hp{hp_vec};
//    YScope sp{sp_map};
//
//    YGC gc{&sp, &hp};
//    gc.mark();
//    EXPECT_EQ(true, fo.is_marked());
//    EXPECT_EQ(true, io.is_marked());
//    for (size_t i = 0; i < lo2.size(); i++) {
//        EXPECT_EQ(false, lo1[i].is_marked());
//        EXPECT_EQ(true, lo2[i].is_marked());
//    }
//}



int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}