// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef GOOGLE_CLOUD_CPP_PUBSUB_GOOGLE_CLOUD_PUBSUB_CREATE_SUBSCRIPTION_BUILDER_H
#define GOOGLE_CLOUD_CPP_PUBSUB_GOOGLE_CLOUD_PUBSUB_CREATE_SUBSCRIPTION_BUILDER_H

#include "google/cloud/pubsub/subscription.h"
#include "google/cloud/pubsub/topic.h"
#include <google/pubsub/v1/pubsub.pb.h>

namespace google {
namespace cloud {
namespace pubsub {
inline namespace GOOGLE_CLOUD_CPP_PUBSUB_NS {

class PushConfigBuilder {
 public:
  explicit PushConfigBuilder(std::string push_endpoint) {
    proto_.set_push_endpoint(std::move(push_endpoint));
  }

  PushConfigBuilder& set_service_account_email(std::string v) {
    proto_.mutable_oidc_token()->set_service_account_email(std::move(v));
    return *this;
  }

  PushConfigBuilder& add_attribute(std::string const& key, std::string const& value) {
    proto_.mutable_attributes()->insert(google::protobuf::Map<std::string, std::string>::value_type(key, value));
    return *this;
  }
  PushConfigBuilder& set_attributes(std::vector<std::pair<std::string, std::string>> attr) {
    google::protobuf::Map<std::string, std::string> attributes;
    for (auto& kv : attr) {
      attributes[kv.first] = std::move(kv.second);
    }
    proto_.mutable_attributes()->swap(attributes);
    return *this;
  }

  google::pubsub::v1::PushConfig as_proto() const& { return proto_; }
  google::pubsub::v1::PushConfig&& as_proto() && { return std::move(proto_); }

 private:
  google::pubsub::v1::PushConfig proto_;
};

/**
 * Create a Cloud Pub/Sub subscription configuration.
 */
class CreateSubscriptionBuilder {
 public:
  explicit CreateSubscriptionBuilder(Subscription const& subscription,
                                     Topic const& topic) {
    proto_.set_name(subscription.FullName());
    proto_.set_topic(topic.FullName());
  }

  CreateSubscriptionBuilder& add_label(std::string const& key,
                                       std::string const& value) {
    using value_type = protobuf::Map<std::string, std::string>::value_type;
    proto_.mutable_labels()->insert(value_type(key, value));
    return *this;
  }

  CreateSubscriptionBuilder& clear_labels() {
    proto_.clear_labels();
    return *this;
  }

  google::pubsub::v1::Subscription as_proto() const& { return proto_; }
  google::pubsub::v1::Subscription&& as_proto() && { return std::move(proto_); }

 private:
  google::pubsub::v1::Subscription proto_;
};

}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub
}  // namespace cloud
}  // namespace google

#endif  // GOOGLE_CLOUD_CPP_PUBSUB_GOOGLE_CLOUD_PUBSUB_CREATE_SUBSCRIPTION_BUILDER_H
