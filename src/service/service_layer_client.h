#ifndef SRC_SERVICE_SERVICE_LAYER_CLIENT_H_
#define SRC_SERVICE_SERVICE_LAYER_CLIENT_H_

#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "chirp_obj.h"
#include "service_layer.grpc.pb.h"

// grpc client to communicate with ServiceLayerServer
class ServiceLayerClient {
 public:
  ServiceLayerClient(std::shared_ptr<grpc::Channel> channel);

  // Register command to send RegisterRequest and receive RegisterReply
  // @uname: username to be registered
  // @ret: true or false based on success
  bool Register(const std::string& uname);

  // Chirp command to send ChirpRequest and receive ChirpReply
  // @uname: user sending Chirp
  // @text: text to be added to Chirp
  // @parent_id: id of Chirp this Chirp is in response to (optional)
  // @ret: the Chirp object that was created
  ChirpObj Chirp(const std::string& uname, const std::string& text,
                 const std::optional<std::string>& parent_id);

  // Follow command to send FollowRequest and receive FollowReply
  // @uname: user logged in
  // @to_follow_user: user that `uname` will follow
  // @ret: true or false based on success
  bool Follow(const std::string& uname, const std::string& to_follow_user);

  // Read command to send ReadRequest and receive ReadReply
  // @chirp_id: id of Chirp to begin reading from
  // @ret: vector of Chirps ordered by thread
  std::vector<ChirpObj> Read(const std::string& chirp_id);

  // Monitor command to send MonitorRequest and receive stream of MonitorReply
  // @uname: user requesting monitor
  // @ret: vector of Chirps to be streamed
  std::vector<ChirpObj> Monitor(const std::string& uname);

  // Monitor command to send MonitorRequest and receive stream of MonitorReply
  // @uname: user requesting monitor
  // @ret: vector of Chirps to be streamed
  std::vector<ChirpObj> Stream(const std::string& uname,const std::string& hash_tag);
 private:
  std::unique_ptr<chirp::ServiceLayer::Stub> stub_;
};

#endif  // SRC_SERVICE_SERVICE_LAYER_CLIENT_H_
