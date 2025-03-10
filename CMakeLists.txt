cmake_minimum_required(VERSION 3.21..3.24)
project(tl-expected
  HOMEPAGE_URL https://tl.tartanllama.xyz
  DESCRIPTION "C++11/14/17 std::expected with functional-style extensions"
  VERSION 1.0.0
  LANGUAGES CXX)

include(CMakeDependentOption)

option(EXPECTED_BUILD_PACKAGE "Build package files as well" ${PROJECT_IS_TOP_LEVEL})

cmake_dependent_option(EXPECTED_BUILD_TESTS
  "Enable tl::expected tests" ${PROJECT_IS_TOP_LEVEL}
  "BUILD_TESTING" OFF)

cmake_dependent_option(EXPECTED_BUILD_RPM
  "Create a RPM" ${PROJECT_IS_TOP_LEVEL}
  "EXPECTED_BUILD_PACKAGE" OFF)

cmake_dependent_option(EXPECTED_BUILD_PACKAGE_DEB
  "Create a DEB" ${PROJECT_IS_TOP_LEVEL}
  "EXPECTED_BUILD_PACKAGE" OFF)

option(EXPECTED_BUILD_INSTALL "Create an install target" ${PROJECT_IS_TOP_LEVEL})

if(EXPECTED_BUILD_INSTALL)
  include(CMakePackageConfigHelpers)
  include(GNUInstallDirs)
endif()

add_library(expected INTERFACE)
target_include_directories(expected
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(expected INTERFACE cxx_std_14)

if (NOT PROJECT_IS_TOP_LEVEL)
  add_library(tl::expected ALIAS expected)
endif()

if(EXPECTED_BUILD_INSTALL)
  # Installation help
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "share/cmake/${PROJECT_NAME}")

  write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT)

  install(TARGETS expected
    EXPORT ${PROJECT_NAME}-targets
    INCLUDES DESTINATION "${CMAKE_INSTALL_DATADIR}")

  install(EXPORT ${PROJECT_NAME}-targets
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
    NAMESPACE tl::
    FILE "${PROJECT_NAME}-targets.cmake")

  install(FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")

  install(DIRECTORY "include/" TYPE INCLUDE)
endif()

if(EXPECTED_BUILD_TESTS)
  include(FetchContent)
  include(CTest)

  enable_testing()
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  set(CATCH_INSTALL_HELPERS OFF)
  set(CATCH_BUILD_TESTING OFF)
  set(CATCH_INSTALL_DOCS OFF)
  FetchContent_Declare(Catch2 URL
    https://github.com/catchorg/Catch2/archive/v2.9.2.zip)
  FetchContent_MakeAvailable(Catch2)

  file(GLOB test-sources CONFIGURE_DEPENDS tests/*.cpp)
  list(FILTER test-sources EXCLUDE REGEX "tests/test.cpp")
  add_executable(${PROJECT_NAME}-tests EXCLUDE_FROM_ALL "${test-sources}")
  target_compile_options(${PROJECT_NAME}-tests PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>)

  target_link_libraries(${PROJECT_NAME}-tests
    PRIVATE
      Catch2::Catch2
      expected)
  add_test(NAME tl::expected::tests COMMAND ${PROJECT_NAME}-tests)
endif()

if(EXPECTED_BUILD_PACKAGE)

  list(APPEND source-generators TBZ2 TGZ TXZ ZIP)

  if (CMAKE_HOST_WIN32)
    list(APPEND binary-generators "WIX")
  endif()

  if (EXPECTED_BUILD_PACKAGE_DEB)
    list(APPEND binary-generators "DEB")
  endif()

  if (EXPECTED_BUILD_RPM)
    list(APPEND binary-generators "RPM")
  endif()


  set(CPACK_SOURCE_GENERATOR ${source-generators})
  set(CPACK_GENERATOR ${binary-generators})

  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")

  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sy Brand")

  list(APPEND CPACK_SOURCE_IGNORE_FILES /.git/ /build/ .gitignore .DS_Store)

  include(CPack)

endif()
