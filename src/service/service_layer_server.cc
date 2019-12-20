#include "service_layer_impl.h"

// Runs the ServiceLayer Server
int main(int argc, char** argv) {
  std::string server_address("0.0.0.0:50002");
  ServiceLayerServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Service Server listening on " << server_address << std::endl;

  server->Wait();

  return 0;
}
