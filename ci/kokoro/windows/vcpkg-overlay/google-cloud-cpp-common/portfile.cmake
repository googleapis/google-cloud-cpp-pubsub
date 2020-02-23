include(vcpkg_common_functions)

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH
    SOURCE_PATH
    REPO
    googleapis/google-cloud-cpp-common
    REF
    v0.20.0
    SHA512
    9b6c26a91cd5fc5d95bcbe7fb8dff106e5bf169ab535a9b6315319800fa0cbd875142a033a3c5ff21df028d221601ffbe8d1feb2349cb156025122abdd715851
    HEAD_REF
    master)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS test BUILD_TESTING)

vcpkg_configure_cmake(
    SOURCE_PATH
    ${SOURCE_PATH}
    PREFER_NINJA
    DISABLE_PARALLEL_CONFIGURE
    OPTIONS
    ${FEATURE_OPTIONS}
    -DGOOGLE_CLOUD_CPP_ENABLE_MACOS_OPENSSL_CHECK=OFF)

vcpkg_install_cmake(ADD_BIN_TO_PATH)

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake TARGET_PATH share)

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)
file(
    INSTALL ${SOURCE_PATH}/LICENSE
    DESTINATION ${CURRENT_PACKAGES_DIR}/share/google-cloud-cpp-common
    RENAME copyright)

vcpkg_copy_pdbs()
