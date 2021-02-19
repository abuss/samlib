cmake_minimum_required(VERSION 3.7.0)

set(PROJECT_AUTHOR "Antal Buss")
set(PROJECT_AUTHOR_EMAIL "antal.buss@gmail.com")

project(samlib VERSION 0.5.0 LANGUAGES CXX)

add_library(samlib INTERFACE)
target_compile_features(samlib INTERFACE cxx_std_20)

set(CMAKE_EXPORT_COMPILE_COMMANDS "1")

message("*")
message("* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message("* Copyright (c) 2020 ${PROJECT_AUTHOR} <${PROJECT_AUTHOR_EMAIL}>")
message("*")

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_PYTHON_BINDING "Build python binding" ON)
option(BUILD_EXAMPLES "Build examples" ON)
# set(QUEUE_IMPLEMENTATION "mycqueue" CACHE STRING "Implementation of concurrent queue to use (concurrentqueue|atomic_queue)")
set(QUEUE_IMPLEMENTATION "concurrentqueue" CACHE STRING "Implementation of concurrent queue to use (concurrentqueue|atomic_queue)")

# set(CMAKE_VERBOSE_MAKEFILE TRUE)

# If no build type is set, set to release.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_SUFFIX)

# External libraries (concurrent queue implementation)
# add_subdirectory(extern)

include(cmake/CPM.cmake)

message("which queue: ${QUEUE_IMPLEMENTATION}")

if ("${QUEUE_IMPLEMENTATION}" STREQUAL "concurrentqueue")
  set(GIT_REPO "cameron314/concurrentqueue")
  set(VERSION 1.0.1)
endif()

if ("${QUEUE_IMPLEMENTATION}" STREQUAL "atomic_queue")
  set(GIT_REPO "max0x7ba/atomic_queue")
endif()

if ("${QUEUE_IMPLEMENTATION}" STREQUAL "MPMCQueue")
  set(GIT_REPO "rigtorp/MPMCQueue")
endif()

CPMAddPackage(
  NAME ConcurrentQueue
  GITHUB_REPOSITORY ${GIT_REPO}
  VERSION ${VERSION}
  DOWNLOAD_ONLY True
)

if (ConcurrentQueue_ADDED)
  add_library(ConcurrentQueue INTERFACE IMPORTED)
  include_directories(${ConcurrentQueue_SOURCE_DIR})
endif()

# PackageProject.cmake will be used to make our target installable
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.0
)

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.3.7
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQUEUE_IMPLEMENTATION_${QUEUE_IMPLEMENTATION}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQUEUE_IMPLEMENTATION_${QUEUE_IMPLEMENTATION} -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pedantic")

# Find threading support
find_package(Threads REQUIRED QUIET)
if(THREADS_FOUND)
    message(STATUS "Found Thread support")
else()
    message(FATAL_ERROR "Can't find Thread support.")
endif()

# # find_library(Atomic-lib atomic)
target_link_libraries(samlib INTERFACE "-latomic")
target_link_libraries(samlib INTERFACE ${COMMON_LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT})

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++20 support has been enabled by default.")

target_include_directories(samlib INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

include(CTest)
enable_testing()


message(STATUS "-------------------------------")
message(STATUS "++ Setup: ")
message(STATUS "System:                 " ${CMAKE_SYSTEM})
message(STATUS "Build dir:              " ${CMAKE_BINARY_DIR})
message(STATUS "Build type:             " ${CMAKE_BUILD_TYPE})
message(STATUS "CXX:                    " ${CMAKE_CXX_COMPILER})
message(STATUS "CXX_ID:                 " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CXXFLAGS:               " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE_SUFFIX}})
message(STATUS "CMAKE_THREAD_LIBS_INIT: " ${CMAKE_THREAD_LIBS_INIT})
message(STATUS "CXX_LINK_FLAGS:         " ${CMAKE_CXX_LINK_FLAGS})
message(STATUS "CMAKE_INCLUDE_PATH:     " ${CMAKE_INCLUDE_PATH})
message(STATUS "Install path:           " ${CMAKE_INSTALL_PREFIX})
message(STATUS "Queue implementation:   " ${QUEUE_IMPLEMENTATION})
message(STATUS "Verbose:                " ${CMAKE_VERBOSE_MAKEFILE})
message(STATUS "")
message(STATUS "++ Options")
message(STATUS "Tests:          " ${BUILD_TESTS})
message(STATUS "Examples:       " ${BUILD_EXAMPLES})
message(STATUS "-------------------------------")


# Tests
if ((BUILD_TESTS) OR (NOT CMAKE_BUILD_TYPE MATCHES "RELEASE"))
  add_subdirectory(test)
endif()

# Python binding
# if (BUILD_PYTHON_BINDING)
#   add_subdirectory(python)
# endif()

# Examples
if (BUILD_EXAMPLES)
 add_subdirectory(examples)
endif()

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
)
