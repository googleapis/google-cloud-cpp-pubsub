//
// Created by coryan on 2/7/20.
//

#include "google/cloud/pubsub/publisher_connection.h"
#include "google/cloud/pubsub/internal/publisher_stub.h"
#include <memory>

namespace google {
namespace cloud {
namespace pubsub {
inline namespace GOOGLE_CLOUD_CPP_PUBSUB_NS {

namespace {
class PublisherConnectionImpl : public PublisherConnection {
 public:
  explicit PublisherConnectionImpl(
      std::shared_ptr<pubsub_internal::PublisherStub> stub)
      : stub_(std::move(stub)) {}

  ~PublisherConnectionImpl() override = default;

  StatusOr<google::pubsub::v1::Topic> CreateTopic(
      CreateTopicParams p) override {
    google::pubsub::v1::Topic request;
    request.set_name("projects/" + p.project_id + "/topics/" + p.topic_id);
    for (auto& kv : p.labels) {
      (*request.mutable_labels())[kv.first] = std::move(kv.second);
    }
    for (auto& r : p.allowed_persistent_regions) {
      request.mutable_message_storage_policy()->add_allowed_persistence_regions(
          std::move(r));
    }
    *request.mutable_kms_key_name() = std::move(p.kms_key_name);
    grpc::ClientContext context;
    return stub_->CreateTopic(context, request);
  }

  Status DeleteTopic(DeleteTopicParams p) override {
    google::pubsub::v1::DeleteTopicRequest request;
    request.set_topic("projects/" + p.project_id + "/topics/" + p.topic_id);
    grpc::ClientContext context;
    return stub_->DeleteTopic(context, request);
  }

 private:
  std::shared_ptr<pubsub_internal::PublisherStub> stub_;
};
}  // namespace

PublisherConnection::~PublisherConnection() = default;

std::shared_ptr<PublisherConnection> MakePublisherConnection(
    ConnectionOptions const& options) {
  auto stub =
      pubsub_internal::CreateDefaultPublisherStub(options, /*channel_id=*/0);
  return std::make_shared<PublisherConnectionImpl>(std::move(stub));
}

}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub
}  // namespace cloud
}  // namespace google
