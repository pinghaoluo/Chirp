#include <gtest/gtest.h>

#include "service_layer.h"

// Tests default constructor for ServiceLayer
TEST(ServiceLayerCreate, NewServiceLayer) {
  ServiceLayer s;
  ASSERT_NE(nullptr, &s);
}

// Tests Register method for ServiceLayer
// Registers a new user
TEST(ServiceLayerRegister, BaseRegister) {
  ServiceLayer s;
  ASSERT_TRUE(s.Register("root"));
}

// Tests Register method for ServiceLayer
// Registers same user twice, should fail on second attempt
TEST(ServiceLayerRegister, DoubleRegister) {
  ServiceLayer s;
  EXPECT_EQ(true, s.Register("root"));
  ASSERT_FALSE(s.Register("root"));
}

// Tests Register method for ServiceLayer
// Registers with empty username, should fail
TEST(ServiceLayerRegister, RegisterEmpty) {
  ServiceLayer s;
  ASSERT_FALSE(s.Register(""));
}

// Tests MakeChirp method for ServiceLayer
// Makes a Chirp to add to DataStore, checks based on returned Chirp object
TEST(ServiceLayerMakeChirp, BaseMakeChirp) {
  ServiceLayer s;
  s.Register("test");
  ChirpObj c = s.MakeChirp("test", "test text", "id");
  ASSERT_FALSE(c.id().empty());

  EXPECT_EQ("test", c.username());
  EXPECT_EQ("test text", c.text());
  EXPECT_EQ("id", c.parent_id());
}

// Tests MakeChirp method for ServiceLayer
// Makes a Chirp with no reply_id, Chirp object returned should not have a
// parent_id
TEST(ServiceLayerMakeChirp, MakeChirpNoReply) {
  ServiceLayer s;
  s.Register("test");
  ChirpObj c = s.MakeChirp("test", "test text", std::nullopt);
  ASSERT_FALSE(c.id().empty());

  EXPECT_EQ("test", c.username());
  EXPECT_EQ("test text", c.text());
  EXPECT_TRUE(c.parent_id().empty());
}

// Tests MakeChirp method for ServiceLayer
// Makes a Chirp with unregistered user. Should fail
TEST(ServiceLayerMakeChirp, MakeChirpUnregisteredUser) {
  ServiceLayer s;
  ChirpObj c = s.MakeChirp("test", "test text", std::nullopt);
  EXPECT_TRUE(c.id().empty());
  EXPECT_TRUE(c.text().empty());
}

// Tests Follow method for ServiceLayer
// `follower` should now follow `root`, based on bool return of Follow()
TEST(ServiceLayerFollow, BaseFollow) {
  ServiceLayer s;
  ASSERT_TRUE(s.Register("root"));
  ASSERT_TRUE(s.Register("follower"));

  EXPECT_TRUE(s.Follow("follower", "root"));
}

// Tests Follow method for ServiceLayer
// `follower` should now follow `root` and `test`
TEST(ServiceLayerFollow, FollowTwoUsers) {
  ServiceLayer s;
  ASSERT_TRUE(s.Register("root"));
  ASSERT_TRUE(s.Register("follower"));
  ASSERT_TRUE(s.Register("test"));

  EXPECT_TRUE(s.Follow("follower", "root"));
  EXPECT_TRUE(s.Follow("follower", "test"));
}

// Tests Follow method for ServiceLayer
// `follower` should now follow `root`
// `test` should now follow `root`
TEST(ServiceLayerFollow, TwoFollowers) {
  ServiceLayer s;
  ASSERT_TRUE(s.Register("root"));
  ASSERT_TRUE(s.Register("follower"));
  ASSERT_TRUE(s.Register("test"));

  EXPECT_TRUE(s.Follow("follower", "root"));
  EXPECT_TRUE(s.Follow("test", "root"));
}

// Tests Follow method for ServiceLayer
// Attempts to follow a non-existent user, should return false
TEST(ServiceLayerFollow, FollowUnregisteredUser) {
  ServiceLayer s;
  ASSERT_TRUE(s.Register("follower"));

  EXPECT_FALSE(s.Follow("follower", "root"));
}

// Tests Follow method for ServiceLayer
// Attempts to call Follow with a non-existent user, should return false
TEST(ServiceLayerFollow, FollowByUnregisteredUser) {
  ServiceLayer s;
  ASSERT_TRUE(s.Register("root"));

  EXPECT_FALSE(s.Follow("follower", "root"));
}

// Tests Read method for ServiceLayer
// Makes 2 Chirps, one in response to the other. Reads the original Chirp.
// Should return vector of Chirps in chrono order
TEST(ServiceLayerRead, BaseRead) {
  ServiceLayer s;
  s.Register("c1");
  s.Register("c2");

  ChirpObj c1 = s.MakeChirp("c1", "c1 text", std::nullopt);
  ASSERT_FALSE(c1.id().empty());
  std::string c1_id = c1.id();
  std::string c1_str = c1.to_string();

  ChirpObj c2 = s.MakeChirp("c2", "c2 text", c1_id);
  ASSERT_FALSE(c2.id().empty());
  std::string c2_str = c2.to_string();

  auto replies = s.Read(c1_id);
  ASSERT_EQ(2, replies.size());
  EXPECT_EQ(c1_str, replies[0].to_string());
  EXPECT_EQ(c2_str, replies[1].to_string());
}

// Tests Read method for ServiceLayer
// Reads from a chirp that has no replies. Should return vector only containing
// the original chirp
TEST(ServiceLayerRead, ReadNoReplies) {
  ServiceLayer s;
  s.Register("c1");

  ChirpObj c1 = s.MakeChirp("c1", "c1 text", std::nullopt);
  ASSERT_FALSE(c1.id().empty());
  std::string c1_id = c1.id();
  std::string c1_str = c1.to_string();

  auto replies = s.Read(c1_id);
  ASSERT_EQ(1, replies.size());
  EXPECT_EQ(c1_str, replies[0].to_string());
}

// Tests Read method for ServiceLayer
// Inserts somes chirps, none replying
// Reads from a chirp that has no replies. Should return vector only containing
// the original chirp
TEST(ServiceLayerRead, ReadNoRepliesTwoUsers) {
  ServiceLayer s;
  s.Register("c1");
  s.Register("c2");

  ChirpObj c1 = s.MakeChirp("c1", "c1 text", std::nullopt);
  ASSERT_FALSE(c1.id().empty());
  std::string c1_id = c1.id();
  std::string c1_str = c1.to_string();

  ChirpObj c2 = s.MakeChirp("c2", "c2 text", std::nullopt);
  ASSERT_FALSE(c2.id().empty());

  ChirpObj c3 = s.MakeChirp("c1", "c3 text", std::nullopt);
  ASSERT_FALSE(c3.id().empty());

  auto replies = s.Read(c1_id);
  ASSERT_EQ(1, replies.size());
  EXPECT_EQ(c1_str, replies[0].to_string());
}

// Tests Read method for ServiceLayer
// Inserts chain of 3 replies and then 1 reply to root chirp.
// Should return vector of replies ordered by 3 chain, then the 1 reply
TEST(ServiceLayerRead, ReadChain) {
  ServiceLayer s;
  s.Register("c1");

  ChirpObj c1 = s.MakeChirp("c1", "c1 text", std::nullopt);
  ASSERT_FALSE(c1.id().empty());
  std::string c1_id = c1.id();
  std::string c1_str = c1.to_string();

  ChirpObj c2 = s.MakeChirp("c1", "c2 text", c1_id);
  ASSERT_FALSE(c2.id().empty());
  std::string c2_id = c2.id();
  std::string c2_str = c2.to_string();

  ChirpObj c3 = s.MakeChirp("c1", "c3 text", c2_id);
  ASSERT_FALSE(c3.id().empty());
  std::string c3_str = c3.to_string();

  ChirpObj c4 = s.MakeChirp("c1", "c4 text", c1_id);
  ASSERT_FALSE(c4.id().empty());
  std::string c4_str = c4.to_string();

  auto replies = s.Read(c1_id);
  ASSERT_EQ(4, replies.size());
  EXPECT_EQ(c1_str, replies[0].to_string());
  EXPECT_EQ(c2_str, replies[1].to_string());
  EXPECT_EQ(c3_str, replies[2].to_string());
  EXPECT_EQ(c4_str, replies[3].to_string());
}

// Tests Monitor function for ServiceLayer
// Due to lack of GRPC, will return vector of store monitor chirps
// `follower` will follow `root` and call Monitor to set up info
// `root` will chirp. `follower` will call Monitor again to retrieve new chirp
// Should only return vector of `root`'s chirps on second call
// Third Monitor call verifies old data is deleted upon retrieval
TEST(ServiceLayerMonitor, BaseMonitor) {
  ServiceLayer s;
  s.Register("root");
  s.Register("follower");
  s.Follow("follower", "root");

  auto check = s.Monitor("follower");
  EXPECT_TRUE(check.empty());

  ChirpObj c1 = s.MakeChirp("root", "test", std::nullopt);
  ASSERT_FALSE(c1.id().empty());

  check = s.Monitor("follower");
  ASSERT_EQ(1, check.size());
  EXPECT_EQ(c1.to_string(), check[0].to_string());

  check = s.Monitor("follower");
  EXPECT_TRUE(check.empty());
}

// Tests Monitor function for ServiceLayer
// Due to lack of GRPC, will return vector of store monitor chirps
// `follower` will follow `root` and call Monitor to set up info
// `root` will chirp twice. `follower` will call Monitor again to retrieve new
// chirps Should only return vector of `root`'s chirps on second call
TEST(ServiceLayerMonitor, BaseMonitorTwoChirps) {
  ServiceLayer s;
  s.Register("root");
  s.Register("follower");
  s.Follow("follower", "root");

  auto check = s.Monitor("follower");
  EXPECT_TRUE(check.empty());

  ChirpObj c1 = s.MakeChirp("root", "test", std::nullopt);
  ASSERT_FALSE(c1.id().empty());

  ChirpObj c2 = s.MakeChirp("root", "test2", std::nullopt);
  ASSERT_FALSE(c2.id().empty());

  check = s.Monitor("follower");
  ASSERT_EQ(2, check.size());
  EXPECT_EQ(c1.to_string(), check[0].to_string());
  EXPECT_EQ(c2.to_string(), check[1].to_string());
}

// Tests Monitor function for ServiceLayer
// Due to lack of GRPC, will return vector of store monitor chirps
// `follower` will follow `root` and `test` and call Monitor to set up info
// `root` and `test` will chirp. `follower` will call Monitor again to retrieve
// new chirps Should only return vector of `root` and `test`'s chirps on second
// call
TEST(ServiceLayerMonitor, BaseMonitorTwoFollowed) {
  ServiceLayer s;
  s.Register("root");
  s.Register("follower");
  s.Register("test");
  s.Follow("follower", "root");
  s.Follow("follower", "test");

  auto check = s.Monitor("follower");
  EXPECT_TRUE(check.empty());

  ChirpObj c1 = s.MakeChirp("root", "root", std::nullopt);
  ASSERT_FALSE(c1.id().empty());

  ChirpObj c2 = s.MakeChirp("test", "test", std::nullopt);
  ASSERT_FALSE(c2.id().empty());

  check = s.Monitor("follower");
  ASSERT_EQ(2, check.size());
  EXPECT_EQ(c1.to_string(), check[0].to_string());
  EXPECT_EQ(c2.to_string(), check[1].to_string());
}

// Tests Monitor function for ServiceLayer
// Due to lack of GRPC, will return vector of store monitor chirps
// `follower` will follow `root` and call Monitor to set up info
// `root` and `test` will chirp. `follower` will call Monitor again to retrieve
// new chirps Should only return vector of `root`'s chirps on second call
TEST(ServiceLayerMonitor, BaseMonitorOnlyOneFollowed) {
  ServiceLayer s;
  s.Register("root");
  s.Register("follower");
  s.Register("test");
  s.Follow("follower", "root");

  auto check = s.Monitor("follower");
  EXPECT_TRUE(check.empty());

  ChirpObj c1 = s.MakeChirp("root", "root", std::nullopt);
  ASSERT_FALSE(c1.id().empty());

  ChirpObj c2 = s.MakeChirp("test", "test", std::nullopt);
  ASSERT_FALSE(c2.id().empty());

  check = s.Monitor("follower");
  ASSERT_EQ(1, check.size());
  EXPECT_EQ(c1.to_string(), check[0].to_string());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
