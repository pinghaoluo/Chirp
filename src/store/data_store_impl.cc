#include "data_store_impl.h"

using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::PutReply;
using chirp::PutRequest;
using grpc::Channel;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

KeyValueStoreServiceImpl::KeyValueStoreServiceImpl() : store_() {}

Status KeyValueStoreServiceImpl::put(ServerContext* context,
                                     const PutRequest* request,
                                     PutReply* response) {
  bool status = store_.Put(request->key(), request->value());
  if (status) {
    return Status::OK;
  }
  return Status::CANCELLED;
}

Status KeyValueStoreServiceImpl::get(
    ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
  GetRequest req;

  while (stream->Read(&req)) {
    std::vector<std::string> vals = store_.Get(req.key());
    for (const std::string& s : vals) {
      GetReply reply;
      reply.set_value(s);
      stream->Write(reply);
    }
  }

  return Status::OK;
}

Status KeyValueStoreServiceImpl::deletekey(ServerContext* context,
                                           const DeleteRequest* request,
                                           DeleteReply* response) {
  if (store_.DeleteKey(request->key())) {
    return Status::OK;
  }

  return Status::CANCELLED;
}
