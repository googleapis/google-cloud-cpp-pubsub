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

// TEST(Subscription, MoveProto) {
//  auto builder = CreateSubscriptionBuilder(Subscription("test-project",
//  "test-subscription"), Topic("test-project", "test-topic"))
//      .add_label("key0", "label0")
//      .add_label("key1", "label1");
//  auto const actual = std::move(builder).as_proto();
//  google::pubsub::v1::Topic expected;
//  ASSERT_TRUE(TextFormat::ParseFromString(
//      R"pb(
//        name: "projects/test-project/topics/test-topic"
//        labels: { key: "key1" value: "label1" }
//        labels: { key: "key0" value: "label0" }
//        message_storage_policy {
//          allowed_persistence_regions: "us-central1"
//          allowed_persistence_regions: "us-west1"
//        }
//        kms_key_name: "projects/.../test-only-string"
//      )pb",
//      &expected));
//  EXPECT_THAT(actual, IsProtoEqual(expected));
//}

}  // namespace
}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub
}  // namespace cloud
}  // namespace google
