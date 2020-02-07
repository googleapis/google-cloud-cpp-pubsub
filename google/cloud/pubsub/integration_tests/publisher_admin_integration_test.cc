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

#include "google/cloud/pubsub/publisher_connection.h"
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

std::string RandomTopic(google::cloud::internal::DefaultPRNG& generator,
                        std::string const& prefix = "cloud-cpp-testing-") {
  return prefix + google::cloud::internal::Sample(generator, 32,
                                                  "abcdefghijklmnopqrstuvwxyz");
}

TEST(PublisherAdminIntegrationTest, PublisherCRUD) {
  auto project_id =
      google::cloud::internal::GetEnv("GOOGLE_CLOUD_PROJECT").value_or("");
  ASSERT_FALSE(project_id.empty());

  auto generator = google::cloud::internal::MakeDefaultPRNG();
  auto topic_id = RandomTopic(generator);

  auto publisher = MakePublisherConnection(ConnectionOptions{});
  auto create_response =
      publisher->CreateTopic({project_id, topic_id,
                              /*.labels*/ {},
                              /*.allowed_persistent_regions*/ {},
                              /*.kms_key_name*/ {}});
  ASSERT_STATUS_OK(create_response);

  auto delete_response = publisher->DeleteTopic({project_id, topic_id});
  ASSERT_STATUS_OK(delete_response);
}

TEST(PublisherAdminIntegrationTest, CreateFailure) {
  auto connection_options =
      ConnectionOptions(grpc::InsecureChannelCredentials())
          .set_endpoint("localhost:1");
  auto publisher = MakePublisherConnection(ConnectionOptions{});
  auto create_response = publisher->CreateTopic(
      {"invalid-project", "invalid-topic",
       /*.labels*/ {{"my-label", "my-value"}},
       /*.allowed_persistent_regions*/ {"us-central1", "us-west1"},
       /*.kms_key_name*/ {}});
  ASSERT_FALSE(create_response);
}

TEST(PublisherAdminIntegrationTest, DeleteFailure) {
  auto connection_options =
      ConnectionOptions(grpc::InsecureChannelCredentials())
          .set_endpoint("localhost:1");
  auto publisher = MakePublisherConnection(ConnectionOptions{});
  auto delete_response =
      publisher->DeleteTopic({"invalid-project", "invalid-topic"});
  ASSERT_FALSE(delete_response.ok());
}

}  // namespace
}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub
}  // namespace cloud
}  // namespace google
