#ifndef SRC_STORE_DATA_STORE_IMPL_H_
#define SRC_STORE_DATA_STORE_IMPL_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "data_store.grpc.pb.h"
#include "data_store.h"

// Servers as DataStore Server. Based on grpc examples
class KeyValueStoreServiceImpl final : public chirp::KeyValueStore::Service {
 public:
  // Default constructor
  // Assumes only one KeyValueStoreServiceImpl will be active at one time
  explicit KeyValueStoreServiceImpl();

  // Handles put request from DataStoreClient
  grpc::Status put(grpc::ServerContext* context,
                   const chirp::PutRequest* request,
                   chirp::PutReply* response) override;

  // Handles get request from DataStoreClient
  grpc::Status get(grpc::ServerContext* context,
                   grpc::ServerReaderWriter<chirp::GetReply, chirp::GetRequest>*
                       stream) override;

  // Handles deletekey request from DataStoreClient
  grpc::Status deletekey(grpc::ServerContext* context,
                         const chirp::DeleteRequest* request,
                         chirp::DeleteReply* response) override;

 private:
  // Actual DataStore object to be communicating with
  DataStore store_;
};

#endif // SRC_STORE_DATA_STORE_IMPL_H_
