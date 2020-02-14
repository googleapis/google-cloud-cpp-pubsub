// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "google/cloud/pubsub/internal/subscriber_stub.h"
#include "google/cloud/pubsub/publisher_client.h"
#include "google/cloud/pubsub/subscription.h"
#include "google/cloud/pubsub/version.h"
#include "google/cloud/internal/getenv.h"
#include "google/cloud/internal/random.h"
#include "google/cloud/testing_util/assert_ok.h"
#include <gmock/gmock.h>

namespace google {
namespace cloud {
namespace pubsub {
inline namespace GOOGLE_CLOUD_CPP_PUBSUB_NS {
namespace {

using ::testing::Contains;
using ::testing::Not;

std::string RandomTopicId(google::cloud::internal::DefaultPRNG& generator,
                          std::string const& prefix = "cloud-cpp-testing-") {
  constexpr int kMaxRandomTopicSuffixLength = 32;
  return prefix + google::cloud::internal::Sample(generator,
                                                  kMaxRandomTopicSuffixLength,
                                                  "abcdefghijklmnopqrstuvwxyz");
}

std::string RandomSubscriptionId(
    google::cloud::internal::DefaultPRNG& generator,
    std::string const& prefix = "cloud-cpp-testing-") {
  constexpr int kMaxRandomTopicSuffixLength = 32;
  return prefix + google::cloud::internal::Sample(generator,
                                                  kMaxRandomTopicSuffixLength,
                                                  "abcdefghijklmnopqrstuvwxyz");
}

TEST(SubscriberAdminIntegrationTest, SubscriberCRUD) {
  auto project_id =
      google::cloud::internal::GetEnv("GOOGLE_CLOUD_PROJECT").value_or("");
  ASSERT_FALSE(project_id.empty());

  auto subscription_names =
      [](std::shared_ptr<pubsub_internal::SubscriberStub> const& stub,
         std::string const& project_id) {
        std::vector<std::string> names;
        grpc::ClientContext context;
        google::pubsub::v1::ListSubscriptionsRequest request;
        request.set_project("projects/" + project_id);
        auto response = stub->ListSubscriptions(context, request);
        EXPECT_STATUS_OK(response);
        if (!response) return names;
        for (auto& s : response->subscriptions()) {
          names.push_back(s.name());
        }
        return names;
      };

  auto generator = google::cloud::internal::MakeDefaultPRNG();
  Topic topic(project_id, RandomTopicId(generator));
  Subscription subscription(project_id, RandomSubscriptionId(generator));

  auto publisher_client = PublisherClient(MakePublisherConnection());
  auto subscriber =
      pubsub_internal::CreateDefaultSubscriberStub(ConnectionOptions{}, 0);

  EXPECT_THAT(subscription_names(subscriber, project_id),
              Not(Contains(subscription.FullName())));

  auto topic_metadata = publisher_client.CreateTopic(CreateTopicBuilder(topic));
  ASSERT_STATUS_OK(topic_metadata);

  struct Deferred {
    std::function<void()> action;
    explicit Deferred(std::function<void()> a) : action(std::move(a)) {}
    ~Deferred() { action(); }
  };
  Deferred cleanup_topic{
      [&publisher_client, &topic] { publisher_client.DeleteTopic(topic); }};

  auto create_response = [&] {
    grpc::ClientContext context;
    google::pubsub::v1::Subscription request;
    request.set_name(subscription.FullName());
    request.set_topic(topic.FullName());
    return subscriber->CreateSubscription(context, request);
  }();
  ASSERT_STATUS_OK(create_response);

  EXPECT_THAT(subscription_names(subscriber, project_id),
              Contains(subscription.FullName()));

  auto delete_response = [&] {
    grpc::ClientContext context;
    google::pubsub::v1::DeleteSubscriptionRequest request;
    request.set_subscription(subscription.FullName());
    return subscriber->DeleteSubscription(context, request);
  }();
  ASSERT_STATUS_OK(delete_response);

  EXPECT_THAT(subscription_names(subscriber, project_id),
              Not(Contains(subscription.FullName())));
}

TEST(SubscriberAdminIntegrationTest, CreateSubscriptionFailure) {
  auto connection_options =
      ConnectionOptions(grpc::InsecureChannelCredentials())
          .set_endpoint("localhost:1");
  auto subscriber =
      pubsub_internal::CreateDefaultSubscriberStub(ConnectionOptions{}, 0);
  grpc::ClientContext context;
  google::pubsub::v1::Subscription request;
  auto create_response = subscriber->CreateSubscription(context, request);
  ASSERT_FALSE(create_response.ok());
}

TEST(SubscriberAdminIntegrationTest, ListSubscriptionsFailure) {
  auto connection_options =
      ConnectionOptions(grpc::InsecureChannelCredentials())
          .set_endpoint("localhost:1");
  auto subscriber =
      pubsub_internal::CreateDefaultSubscriberStub(ConnectionOptions{}, 0);
  grpc::ClientContext context;
  google::pubsub::v1::ListSubscriptionsRequest request;
  auto list_response = subscriber->ListSubscriptions(context, request);
  ASSERT_FALSE(list_response.ok());
}

TEST(SubscriberAdminIntegrationTest, DeleteSubscriptionFailure) {
  auto connection_options =
      ConnectionOptions(grpc::InsecureChannelCredentials())
          .set_endpoint("localhost:1");
  auto subscriber =
      pubsub_internal::CreateDefaultSubscriberStub(ConnectionOptions{}, 0);
  grpc::ClientContext context;
  google::pubsub::v1::DeleteSubscriptionRequest request;
  auto delete_response = subscriber->DeleteSubscription(context, request);
  ASSERT_FALSE(delete_response.ok());
}

}  // namespace
}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub
}  // namespace cloud
}  // namespace google
