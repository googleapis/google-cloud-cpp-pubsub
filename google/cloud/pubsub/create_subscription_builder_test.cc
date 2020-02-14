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

#include "create_subscription_builder.h"
#include "google/cloud/pubsub/create_topic_builder.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>
#include <gmock/gmock.h>
#include <sstream>

namespace google {
namespace cloud {
namespace pubsub {
inline namespace GOOGLE_CLOUD_CPP_PUBSUB_NS {
namespace {

using ::google::protobuf::TextFormat;

// TODO(#86) - use the version from `-common` when it is ready.
MATCHER_P(IsProtoEqual, value, "Checks whether protos are equal") {
  std::string delta;
  google::protobuf::util::MessageDifferencer differencer;
  differencer.ReportDifferencesToString(&delta);
  auto const result = differencer.Compare(arg, value);
  *result_listener << "\n" << delta;
  return result;
}

TEST(CreateSubscriptionBuilder, MakeOidcToken) {
  auto const actual =
      PushConfigBuilder::MakeOidcToken("test-account@example.com");
  google::pubsub::v1::PushConfig::OidcToken expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        service_account_email: "test-account@example.com"
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, MakeOidcTokenWithAudience) {
  auto const actual = PushConfigBuilder::MakeOidcToken(
      "test-account@example.com", "test-audience");
  google::pubsub::v1::PushConfig::OidcToken expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        service_account_email: "test-account@example.com"
        audience: "test-audience"
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, PushConfigBasic) {
  auto const actual =
      PushConfigBuilder("https://endpoint.example.com").as_proto();
  google::pubsub::v1::PushConfig expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        push_endpoint: "https://endpoint.example.com"
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, PushConfigAddAttribute) {
  auto const actual = PushConfigBuilder("https://endpoint.example.com")
                          .add_attribute("key0", "label0")
                          .add_attribute("key1", "label1")
                          .as_proto();
  google::pubsub::v1::PushConfig expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        push_endpoint: "https://endpoint.example.com"
        attributes: { key: "key1" value: "label1" }
        attributes: { key: "key0" value: "label0" }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, PushConfigSetAttributes) {
  auto const actual = PushConfigBuilder("https://endpoint.example.com")
                          .add_attribute("key0", "label0")
                          .add_attribute("key1", "label1")
                          .set_attributes({{"key2", "label2"}})
                          .as_proto();
  google::pubsub::v1::PushConfig expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        push_endpoint: "https://endpoint.example.com"
        attributes: { key: "key2" value: "label2" }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, PushConfigSetAuthentication) {
  auto const actual =
      PushConfigBuilder("https://endpoint.example.com")
          .set_authentication(PushConfigBuilder::MakeOidcToken(
              "fake-service-account@example.com", "test-audience"))
          .as_proto();
  google::pubsub::v1::PushConfig expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        push_endpoint: "https://endpoint.example.com"
        oidc_token {
          service_account_email: "fake-service-account@example.com"
          audience: "test-audience"
        }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, Basic) {
  auto const actual = CreateSubscriptionBuilder(
                          Subscription("test-project", "test-subscription"),
                          Topic("test-project", "test-topic"))
                          .as_proto();
  google::pubsub::v1::Subscription expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        name: "projects/test-project/subscriptions/test-subscription"
        topic: "projects/test-project/topics/test-topic"
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, SetPushConfig) {
  auto const actual =
      CreateSubscriptionBuilder(
          Subscription("test-project", "test-subscription"),
          Topic("test-project", "test-topic"))
          .set_push_config(
              PushConfigBuilder("https://ep.example.com").as_proto())
          .as_proto();
  google::pubsub::v1::Subscription expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        name: "projects/test-project/subscriptions/test-subscription"
        topic: "projects/test-project/topics/test-topic"
        push_config { push_endpoint: "https://ep.example.com" }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, AddLabels) {
  auto const actual = CreateSubscriptionBuilder(
                          Subscription("test-project", "test-subscription"),
                          Topic("test-project", "test-topic"))
                          .add_label("key0", "label0")
                          .add_label("key1", "label1")
                          .as_proto();
  google::pubsub::v1::Subscription expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        name: "projects/test-project/subscriptions/test-subscription"
        topic: "projects/test-project/topics/test-topic"
        labels: { key: "key0", value: "label0" }
        labels: { key: "key1", value: "label1" }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, SetLabels) {
  auto const actual = CreateSubscriptionBuilder(
                          Subscription("test-project", "test-subscription"),
                          Topic("test-project", "test-topic"))
                          .add_label("key0", "label0")
                          .add_label("key1", "label1")
                          .set_labels({{"key2", "label2"}})
                          .as_proto();
  google::pubsub::v1::Subscription expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        name: "projects/test-project/subscriptions/test-subscription"
        topic: "projects/test-project/topics/test-topic"
        labels: { key: "key2", value: "label2" }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

TEST(CreateSubscriptionBuilder, ClearLabels) {
  auto const actual = CreateSubscriptionBuilder(
                          Subscription("test-project", "test-subscription"),
                          Topic("test-project", "test-topic"))
                          .add_label("key0", "label0")
                          .clear_labels()
                          .add_label("key1", "label1")
                          .as_proto();
  google::pubsub::v1::Subscription expected;
  ASSERT_TRUE(TextFormat::ParseFromString(
      R"pb(
        name: "projects/test-project/subscriptions/test-subscription"
        topic: "projects/test-project/topics/test-topic"
        labels: { key: "key1", value: "label1" }
      )pb",
      &expected));
  EXPECT_THAT(actual, IsProtoEqual(expected));
}

}  // namespace
}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub
}  // namespace cloud
}  // namespace google
