#ifndef SRC_STORE_DATA_STORE_CLIENT_H_
#define SRC_STORE_DATA_STORE_CLIENT_H_

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "data_store.grpc.pb.h"

// grpc client to communicate with DataStoreServer
class DataStoreClient {
 public:
  // Default constructor for DataStoreClient
  DataStoreClient(std::shared_ptr<grpc::Channel> channel);

  // Put command to send PutRequest and receive PutReply
  // @key: key to be put
  // @val: val to be put at 'key'
  // @ret: true on successful insertion
  bool Put(const std::string& key, const std::string& val);

  // Get command to send GetRequest and receive GetReply
  // @key: key to be retrieved from
  // @ret: value retrieved from DataStore
  std::vector<std::string> Get(const std::string& key);

  // DeleteKey command to send DeleteRequest and receive DeleteReply
  // @key: key to be deleted
  // @ret: true on successful deletetion
  bool DeleteKey(const std::string& key);

 private:
  std::unique_ptr<chirp::KeyValueStore::Stub> stub_;
};

#endif  // SRC_STORE_DATA_STORE_CLIENT_H_
