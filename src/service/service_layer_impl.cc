#include "service_layer_impl.h"

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
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

ServiceLayerServiceImpl::ServiceLayerServiceImpl() : service_() {}

Status ServiceLayerServiceImpl::registeruser(ServerContext* context,
                                             const RegisterRequest* request,
                                             RegisterReply* response) {
  bool status = service_.Register(request->username());
  if (status) {
    return Status::OK;
  }
  return Status::CANCELLED;
}

Status ServiceLayerServiceImpl::chirp(ServerContext* context,
                                      const ChirpRequest* request,
                                      ChirpReply* response) {
  // Handles if ChirpObj reply_id value
  std::optional<std::string> reply_id =
      request->parent_id().empty()
          ? std::nullopt
          : std::optional<std::string>{request->parent_id()};

  ChirpObj chirp =
      service_.MakeChirp(request->username(), request->text(), reply_id);
  if (chirp.username().empty()) {
    return Status::CANCELLED;
  }

  Chirp* reply = response->mutable_chirp();
  reply->set_username(chirp.username());
  reply->set_text(chirp.text());
  reply->set_id(chirp.id());
  reply->set_parent_id(chirp.parent_id());
  reply->mutable_timestamp()->set_seconds(chirp.time().seconds);
  reply->mutable_timestamp()->set_useconds(chirp.time().useconds);

  return Status::OK;
}

Status ServiceLayerServiceImpl::follow(ServerContext* context,
                                       const FollowRequest* request,
                                       FollowReply* response) {
  bool status = service_.Follow(request->username(), request->to_follow());

  if (status) {
    return Status::OK;
  }

  return Status::CANCELLED;
}

Status ServiceLayerServiceImpl::read(ServerContext* context,
                                     const ReadRequest* request,
                                     ReadReply* response) {
  std::vector<ChirpObj> reply_chirps = service_.Read(request->chirp_id());
  for (const auto& c : reply_chirps) {
    Chirp* reply = response->add_chirps();
    reply->set_username(c.username());
    reply->set_text(c.text());
    reply->set_id(c.id());
    reply->set_parent_id(c.parent_id());
    reply->mutable_timestamp()->set_seconds(c.time().seconds);
    reply->mutable_timestamp()->set_useconds(c.time().useconds);
  }

  return Status::OK;
}

Status ServiceLayerServiceImpl::monitor(ServerContext* context,
                                        const MonitorRequest* request,
                                        ServerWriter<MonitorReply>* writer) {
  std::vector<ChirpObj> chirps = service_.Monitor(request->username());
  for (const ChirpObj& c : chirps) {
    MonitorReply reply;
    Chirp* reply_chirp = reply.mutable_chirp();
    reply_chirp->set_username(c.username());
    reply_chirp->set_text(c.text());
    reply_chirp->set_id(c.id());
    reply_chirp->set_parent_id(c.parent_id());
    reply_chirp->mutable_timestamp()->set_seconds(c.time().seconds);
    reply_chirp->mutable_timestamp()->set_useconds(c.time().useconds);

    writer->Write(reply);
  }

  return Status::OK;
}

Status ServiceLayerServiceImpl::hash(ServerContext* context,
                                        const HashRequest* request,
                                        ServerWriter<HashReply>* writer) {
  std::vector<ChirpObj> chirps = service_.HashTag(request->username(),request->hashtag());
  for (const ChirpObj& c : chirps) {
    HashReply reply;
    Chirp* reply_chirp = reply.mutable_chirp();
    reply_chirp->set_username(c.username());
    reply_chirp->set_text(c.text());
    reply_chirp->set_id(c.id());
    reply_chirp->set_parent_id(c.parent_id());
    reply_chirp->mutable_timestamp()->set_seconds(c.time().seconds);
    reply_chirp->mutable_timestamp()->set_useconds(c.time().useconds);

    writer->Write(reply);
  }

  return Status::OK;
}