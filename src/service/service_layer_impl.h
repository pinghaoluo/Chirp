#ifndef SRC_SERVICE_SERVICE_LAYER_IMPL_H_
#define SRC_SERVICE_SERVICE_LAYER_IMPL_H_ 

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "service_layer.grpc.pb.h"
#include "service_layer_grpc.h"

class ServiceLayerServiceImpl final : public chirp::ServiceLayer::Service {
 public:
  // Default constructor
  explicit ServiceLayerServiceImpl();

  // Handles register command received from ServiceLayerClient
  grpc::Status registeruser(grpc::ServerContext* context,
                            const chirp::RegisterRequest* request,
                            chirp::RegisterReply* response) override;

  // Handles chirp command received from ServiceLayerClient
  grpc::Status chirp(grpc::ServerContext* context,
                     const chirp::ChirpRequest* request,
                     chirp::ChirpReply* response) override;

  // Handles follow command received from ServiceLayerClient
  grpc::Status follow(grpc::ServerContext* context,
                      const chirp::FollowRequest* request,
                      chirp::FollowReply* response) override;

  // Handles read command received from ServiceLayerClient
  grpc::Status read(grpc::ServerContext* context,
                    const chirp::ReadRequest* request,
                    chirp::ReadReply* response) override;

  // Handles monitor command received from ServiceLayerClient
  grpc::Status monitor(
      grpc::ServerContext* context, const chirp::MonitorRequest* request,
      grpc::ServerWriter<chirp::MonitorReply>* writer) override;

  // Handles stream command received from ServiceLayerClient
  grpc::Status hash(
    grpc::ServerContext* context, const chirp::HashRequest* request,
    grpc::ServerWriter<chirp::HashReply>* writer) override;

 private:
  // ServiceLayerObj the server is interacting with
  ServiceLayerObj service_;
};

#endif // SRC_SERVICE_SERVICE_LAYER_IMPL_H_
