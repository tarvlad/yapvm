#include <gtest/gtest.h>
#include "kvstorage.h"
#include "scope.h"

using namespace yapvm::interpreter;

TEST(kv_storage_test, correctness_test) {
    KVStorage<std::string, yapvm::interpreter::ScopeEntry> kv_storage;

    kv_storage.add("greet", yapvm::interpreter::ScopeEntry{nullptr, FUNCTION });

    EXPECT_TRUE(kv_storage.find("greet").has_value());
}
