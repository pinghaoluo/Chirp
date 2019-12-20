#include <gtest/gtest.h>

#include "data_store.h"

// Tests default constructor for DataStore
TEST(DataStoreCreate, NewDataStore) {
  DataStore ds;
  ASSERT_NE(nullptr, &ds);
}

// Tests Put method for DataStore
// Single Put and verifies through Put return val
TEST(DataStorePut, BasePut) {
  DataStore ds;
  bool check = ds.Put("test", "test_value");
  EXPECT_TRUE(check);
}

// Tests Put for DataStore at same key
// Vector at 'key' should have 2 entries
TEST(DataStorePut, PutSameKey) {
  DataStore ds;
  ds.Put("test", "1");
  ASSERT_EQ("1", ds.Get("test")[0]);

  bool check = ds.Put("test", "2");
  EXPECT_TRUE(check);
  EXPECT_EQ(2, ds.Get("test").size());
  EXPECT_EQ("2", ds.Get("test")[1]);
}

// Tests Get for DataStore
// Checks value at 'key' is what is Put in
TEST(DataStoreGet, BaseGet) {
  DataStore ds;
  ds.Put("test", "1");
  EXPECT_EQ("1", ds.Get("test")[0]);
}

// Tests Get for DataStore with invalid 'key'
// Should return an empty vector
TEST(DataStoreGet, GetNoKey) {
  DataStore ds;
  auto check = ds.Get("test");
  ASSERT_EQ(true, check.empty());
}

// Tests DeleteKey for DataStore
// Should return true signalling successful delete
// Should cause all entries at 'key' to be removed, returning empty vector
TEST(DataStoreDel, BaseDel) {
  DataStore ds;
  ds.Put("test", "1");

  EXPECT_EQ(true, ds.DeleteKey("test"));
  EXPECT_EQ(true, ds.Get("test").empty());
}

// Tests DeleteKey for DataStore on invalid 'key'
// Should return false, signalling failure
TEST(DataStoreDel, DelNoKey) {
  DataStore ds;
  EXPECT_FALSE(ds.DeleteKey("test"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
