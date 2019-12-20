#include "service_layer_client.h"

using chirp::Chirp;
using chirp::ChirpReply;
using chirp::ChirpRequest;
using chirp::FollowReply;
using chirp::FollowRequest;
using chirp::MonitorReply;
using chirp::MonitorRequest;
using chirp::HashReply;
using chirp::HashRequest;
using chirp::ReadReply;
using chirp::ReadRequest;
using chirp::RegisterReply;
using chirp::RegisterRequest;
using chirp::ServiceLayer;
using chirp::Timestamp;
using chirp::Users;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

ServiceLayerClient::ServiceLayerClient(std::shared_ptr<Channel> channel)
    : stub_(ServiceLayer::NewStub(channel)) {}

bool ServiceLayerClient::Register(const std::string& uname) {
  RegisterRequest request;
  request.set_username(uname);

  RegisterReply reply;
  ClientContext context;

  Status status = stub_->registeruser(&context, request, &reply);
  return status.ok();
}

ChirpObj ServiceLayerClient::Chirp(
    const std::string& uname, const std::string& text,
    const std::optional<std::string>& parent_id) {
  ChirpRequest request;
  request.set_username(uname);
  request.set_text(text);
  request.set_parent_id(parent_id.value_or(""));

  ChirpReply reply;
  ClientContext context;

  Status status = stub_->chirp(&context, request, &reply);
  if (status.ok()) {
    auto chirp = reply.chirp();
    return ChirpObj(chirp.username(), chirp.text(), chirp.id(),
                    chirp.parent_id(), chirp.timestamp().seconds(),
                    chirp.timestamp().useconds());
  }
  return ChirpObj();
}

bool ServiceLayerClient::Follow(const std::string& uname,
                                const std::string& to_follow_user) {
  FollowRequest request;
  request.set_username(uname);
  request.set_to_follow(to_follow_user);

  FollowReply reply;
  ClientContext context;

  Status status = stub_->follow(&context, request, &reply);
  return status.ok();
}

std::vector<ChirpObj> ServiceLayerClient::Read(const std::string& chirp_id) {
  ReadRequest request;
  request.set_chirp_id(chirp_id);

  ReadReply reply;
  ClientContext context;

  Status status = stub_->read(&context, request, &reply);
  std::vector<ChirpObj> replies;
  for (const auto& chirp : reply.chirps()) {
    ChirpObj c(chirp.username(), chirp.text(), chirp.id(), chirp.parent_id(),
               chirp.timestamp().seconds(), chirp.timestamp().useconds());
    replies.push_back(c);
  }
  return replies;
}

std::vector<ChirpObj> ServiceLayerClient::Monitor(const std::string& uname) {
  MonitorRequest request;
  request.set_username(uname);
  ClientContext context;
  std::shared_ptr<ClientReader<MonitorReply> > stream(
      stub_->monitor(&context, request));

  MonitorReply reply;
  std::vector<ChirpObj> chirps;
  while (stream->Read(&reply)) {
    auto chirp = reply.chirp();
    ChirpObj c(chirp.username(), chirp.text(), chirp.id(), chirp.parent_id(),
               chirp.timestamp().seconds(), chirp.timestamp().useconds());
    chirps.push_back(c);
  }
  return chirps;
}

std::vector<ChirpObj> ServiceLayerClient::Stream(const std::string& uname,const std::string& hash_tag) {
  HashRequest request;
  request.set_username(uname);
  request.set_hashtag(hash_tag);
  ClientContext context;
  std::shared_ptr<ClientReader<HashReply> > stream(
      stub_->hash(&context, request));

  HashReply reply;
  std::vector<ChirpObj> chirps;
  while (stream->Read(&reply)) {
    auto chirp = reply.chirp();
    ChirpObj c(chirp.username(), chirp.text(), chirp.id(), chirp.parent_id(),
               chirp.timestamp().seconds(), chirp.timestamp().useconds());
    chirps.push_back(c);
  }
  return chirps;
}