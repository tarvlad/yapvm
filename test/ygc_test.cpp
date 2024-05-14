#include <gtest/gtest.h>
#include "y_objects.h"
#include "scope.h"
#include "gc.h"
#include "thread_manager.h"

using namespace yapvm;
using namespace yobjects;
using namespace ygc;
using namespace interpreter;

TEST(gc_test, mark_all) {

    Scope scope;

    ManagedObject *m_int = new ManagedObject { constr_yint(42) };
    ManagedObject *string_field = new ManagedObject { constr_ystring("hello!") };
    ManagedObject *obj = new ManagedObject { constr_yobject("Greeter") };
    obj->value()->add_field("greeting", string_field);

    // x = 41
    // class Greeter:
    //    greeting = hello!
    scope.add_object("x", m_int);
    scope.add_object("Greeter", obj);    

    ThreadManager tm;
    YGC gc (&scope, &tm);
    gc.mark();

    EXPECT_TRUE(m_int->is_marked());
    EXPECT_TRUE(obj->is_marked());
    EXPECT_TRUE(string_field->is_marked());
}


TEST(gc_test, unreacheble) {

    // f = 3.1415
    // a = 42
    // list1 = ['1', '2']
    // list2 = ['3', '4']
    // list2 = list1

    // so list2 and it's kids are unreachable

    Scope scope;

    ManagedObject *f_obj = new ManagedObject {constr_yfloat(3.1415)};    
    ManagedObject *a_obj = new ManagedObject {constr_yint(42)};    
    scope.add_object("f", f_obj);
    scope.add_object("a", a_obj);

    auto vec1 = new std::vector<ManagedObject *> { new ManagedObject {constr_yint(1)}, new ManagedObject {constr_yint(2)}};
    YObject *list1 = constr_ylist(vec1);

    auto vec2 = new std::vector<ManagedObject *> { new ManagedObject {constr_yint(3)}, new ManagedObject {constr_yint(4)}};
    YObject *list2 = constr_ylist(vec2);

    ManagedObject *list1_obj = new ManagedObject { list1 };
    ManagedObject *list2_obj = new ManagedObject { list2 };
    scope.add_object("list1", list1_obj);    
    scope.add_object("list2", list2_obj);    

    // list2 = list1 now list2 are unreachable
    scope.change("list2", *(scope.get("list1")));

    ThreadManager tm;
    YGC gc (&scope, &tm);
    gc.mark();

    // unreachable object is unmarked
    EXPECT_FALSE(list2_obj->is_marked());
    for (ManagedObject *obj : *vec2) {
        EXPECT_FALSE(obj->is_marked());
    }

    // all live objects are reachable
    EXPECT_TRUE(f_obj->is_marked());
    EXPECT_TRUE(a_obj->is_marked());
    EXPECT_TRUE(list1_obj->is_marked());
    for (ManagedObject *obj : *vec1) {
        EXPECT_TRUE(obj->is_marked());
    }
}

TEST(gc_test, nested_lists) {

    // l1 = [1, 2]
    // l2 = [3, 4, l1]
    
    Scope scope;
    auto vec1 = new std::vector<ManagedObject *> { new ManagedObject {constr_yint(1)}, new ManagedObject {constr_yint(2)}};
    YObject *list1 = constr_ylist(vec1);

    auto vec2 = new std::vector<ManagedObject *> { new ManagedObject {constr_yint(3)}, new ManagedObject {constr_yint(4)}, new ManagedObject {list1} };
    YObject *list2 = constr_ylist(vec2);

    ManagedObject *list2_obj = new ManagedObject { list2 };
    scope.add_object("list2", list2_obj);    

    ThreadManager tm;
    YGC gc (&scope, &tm);
    gc.mark();

    EXPECT_TRUE(list2_obj->is_marked());

    for (ManagedObject *obj : *vec2) {
        EXPECT_TRUE(obj->is_marked());
    }

    for (ManagedObject *obj : *vec1) {
        EXPECT_TRUE(obj->is_marked());
    }
}

TEST(gc_test, cycle_dependency) {
    // class Foo():
    //     def __init__(self, l):
    //         self.l = l
    // l = []
    // f = Foo(l)
    // f.l.append(f)

    Scope scope;
    YObject *list = constr_ylist();
    ManagedObject *list_obj = new ManagedObject { list };

    YObject *foo = constr_yobject("Foo");
    ManagedObject *foo_obj = new ManagedObject { foo };

    foo->add_field("l", list_obj);

    scope.add_object("foo", foo_obj);
    scope.add_object("l", list_obj);

    ThreadManager tm;
    YGC gc (&scope, &tm);
    gc.mark();

    EXPECT_TRUE(foo_obj->is_marked());
    EXPECT_TRUE(list_obj->is_marked());
}

TEST(gc_test, sweep) {    
    Scope scope;

    ThreadManager tm;
    YGC gc (&scope, &tm);

    ManagedObject *f_obj = new ManagedObject {constr_yfloat(3.1415)};    
    ManagedObject *a_obj = new ManagedObject {constr_yint(42)};    
    scope.add_object("f", f_obj);
    scope.add_object("a", a_obj);    

    std::vector<ManagedObject *> objects {GC_CASH_LIMIT + 3};

    for (size_t i = 0; i < objects.size(); i++) {
        objects[i] = new ManagedObject { constr_yint(i) };
    }
    objects.push_back(a_obj);
    objects.push_back(f_obj);
    gc.fill_left(objects);

    gc.mark();
    EXPECT_TRUE(a_obj->is_marked());
    EXPECT_TRUE(f_obj->is_marked());

    gc.sweep();
    EXPECT_EQ(2, gc.left().size());
    EXPECT_FALSE(a_obj->is_marked());
    EXPECT_FALSE(f_obj->is_marked());


    for (size_t i = 0; i < objects.size(); i++) {
        objects[i] = new ManagedObject { constr_yint(i) };
    }

    gc.fill_left(objects);
    scope.change("f", *(scope.get("a")));

    gc.mark();
    gc.sweep();

    EXPECT_EQ(1, gc.left().size());
    EXPECT_EQ(42, gc.left()[0]->value()->get_value_as_int());
}



int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
