# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# A minimal CMakeList.txt to *test* the install Cloud Pub/Sub C++ client.
#
# This is not intended to be a demonstration of how to write good CMakeList.txt
# files, nor is it a general solution to create CMake files for google-cloud-
# cpp-pubsub. It is simply a minimal CMake file to verify that (a) the
# installed libraries contain all required headers, and (b) the installed CMake
# configuration files are usable.

cmake_minimum_required(VERSION 3.5)
project(google-cloud-cpp-pubsub-install-integration-test CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(google_cloud_cpp_pubsub_client REQUIRED)

# Once the pubsub_client package is found, define new targets.
add_executable(pubsub_install_test pubsub_install_test.cc)
target_link_libraries(pubsub_install_test googleapis-c++::pubsub_client)
