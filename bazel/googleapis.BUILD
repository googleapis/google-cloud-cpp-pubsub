# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

cc_proto_library(
    name = "pubsub_cc_proto",
    deps = ["//google/pubsub/v1:pubsub_proto"],
)

cc_grpc_library(
    name = "grpc_pubsub_proto",
    srcs = ["//google/pubsub/v1:pubsub_proto"],
    grpc_only = True,
    use_external = True,
    well_known_protos = True,
    deps = [":pubsub_cc_proto"],
)

cc_library(
    name = "pubsub_protos",
    includes = ["."],
    deps = [
        ":grpc_pubsub_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_library(
    name = "grpc_utils_protos",
    includes = [
        ".",
    ],
    deps = [
        "//google/rpc:status_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
