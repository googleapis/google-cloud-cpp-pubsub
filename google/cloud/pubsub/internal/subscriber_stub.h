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

#ifndef GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_PUBSUB_INTERNAL_SUBSCRIBER_STUB_H
#define GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_PUBSUB_INTERNAL_SUBSCRIBER_STUB_H

#include "google/cloud/pubsub/connection_options.h"
#include "google/cloud/status_or.h"
#include <google/pubsub/v1/pubsub.grpc.pb.h>

namespace google {
namespace cloud {
namespace pubsub_internal {
inline namespace GOOGLE_CLOUD_CPP_PUBSUB_NS {

/**
 * Define the interface for the gRPC wrapper.
 *
 * We wrap the gRPC-generated `SubscriberStub` to:
 *   - Return a StatusOr<T> instead of using a `grpc::Status` and an "output
 *     parameter" for the response.
 *   - To be able to mock the stubs.
 *   - To be able to decompose some functionality (logging, adding metadata
 *     information) into layers.
 */
class SubscriberStub {
 public:
  virtual ~SubscriberStub() = default;

  /// Create a new subscription.
  virtual StatusOr<google::pubsub::v1::Subscription> CreateSubscription(
      grpc::ClientContext& client_context,
      google::pubsub::v1::Subscription const& request) = 0;

  /// List existing subscriptions.
  virtual StatusOr<google::pubsub::v1::ListSubscriptionsResponse>
  ListSubscriptions(
      grpc::ClientContext& client_context,
      google::pubsub::v1::ListSubscriptionsRequest const& request) = 0;

  /// Delete a subscription.
  virtual Status DeleteSubscription(
      grpc::ClientContext& client_context,
      google::pubsub::v1::DeleteSubscriptionRequest const& request) = 0;
};

/**
 * Creates a SubscriberStub configured with @p options and @p channel_id.
 *
 * @p channel_id should be unique among all stubs in the same Connection pool,
 * to ensure they use different underlying connections.
 */
std::shared_ptr<SubscriberStub> CreateDefaultSubscriberStub(
    pubsub::ConnectionOptions const& options, int channel_id);

}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub_internal
}  // namespace cloud
}  // namespace google

#endif  // GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_PUBSUB_INTERNAL_SUBSCRIBER_STUB_H
