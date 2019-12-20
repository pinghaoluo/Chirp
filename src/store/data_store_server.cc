#include "data_store_impl.h"

// Runs the KeyValueStoreService Server
int main(int argc, char** argv) {
  std::string server_address("0.0.0.0:50000");
  KeyValueStoreServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "KeyValue Server listening on " << server_address << std::endl;

  server->Wait();

  return 0;
}
