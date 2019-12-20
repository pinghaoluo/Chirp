#include "data_store_client.h"

using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::KeyValueStore;
using chirp::PutReply;
using chirp::PutRequest;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

// Helper method to make a GetRequest
GetRequest MakeGetRequest(const std::string& key) {
  GetRequest r;
  r.set_key(key);
  return r;
}

DataStoreClient::DataStoreClient(std::shared_ptr<Channel> channel)
    : stub_(KeyValueStore::NewStub(channel)) {}

bool DataStoreClient::Put(const std::string& key, const std::string& val) {
  PutRequest request;
  request.set_key(key);
  request.set_value(val);

  PutReply reply;
  ClientContext context;

  Status status = stub_->put(&context, request, &reply);
  return status.ok();
}

std::vector<std::string> DataStoreClient::Get(const std::string& key) {
  ClientContext context;
  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
      stub_->get(&context));

  std::thread writer([stream, key]() {
    stream->Write(MakeGetRequest(key));
    stream->WritesDone();
  });

  GetReply reply;
  std::vector<std::string> replies;
  while (stream->Read(&reply)) {
    replies.push_back(reply.value());
  }
  writer.join();
  Status status = stream->Finish();

  if (status.ok()) {
    return replies;
  }

  return std::vector<std::string>{};
}

bool DataStoreClient::DeleteKey(const std::string& key) {
  DeleteRequest request;
  request.set_key(key);

  DeleteReply reply;
  ClientContext context;

  Status status = stub_->deletekey(&context, request, &reply);
  return status.ok();
}
