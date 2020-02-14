cmake_minimum_required(VERSION 3.7.0)
project(samlib VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS "1")

set(PROJECT_AUTHOR "Antal Buss")
set(PROJECT_AUTHOR_EMAIL "antal.buss@gmail.com")

message("*")
message("* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message("* Copyright (c) 2019 ${PROJECT_AUTHOR} <${PROJECT_AUTHOR_EMAIL}>")
message("*")

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_PYTHON_BINDING "Build python binding" ON)
option(BUILD_EXAMPLES "Build examples" ON)
set(QUEUE_IMPLEMENTATION "concurrentqueue" CACHE STRING "Implementation of concurrent queue to use (concurrentqueue|atomic_queue)")

# set(CMAKE_VERBOSE_MAKEFILE TRUE)


add_library(samlib INTERFACE)

# If no build type is set, set to release.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_SUFFIX)

# External libraries (concurrent queue implementation)
add_subdirectory(extern)

include_directories(samlib INTERFACE src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQUEUE_IMPLEMENTATION_${QUEUE_IMPLEMENTATION}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pedantic")

# Find threading support
find_package(Threads REQUIRED QUIET)
if(THREADS_FOUND)
    message(STATUS "Found Thread support")
else()
    message(FATAL_ERROR "Can't find Thread support.")
endif()

# find_library(Atomic-lib atomic)
target_link_libraries(samlib INTERFACE "-latomic")
target_link_libraries(samlib INTERFACE ${COMMON_LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++17 support has been enabled by default.")

target_include_directories(samlib INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

include(CTest)
enable_testing()

# if (BUILD_DOCS)
#     # check if Doxygen is installed
#     find_package(Doxygen)

#     if (DOXYGEN_FOUND)
#         # # set input and output files
#         # set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in)
#         set(DOXYGEN_CONF ${CMAKE_SOURCE_DIR}/doc/Doxyfile)

#         # # request to configure the file
#         # configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#         # message("Doxygen build started")

#         set(SPHINX_HTML ON)
#         # note the option ALL which allows to build the docs together with the application
#         add_custom_target( doc_doxygen ALL
#             COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONF}
#             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#             COMMENT "Generating API documentation with Doxygen"
#             VERBATIM )
#     else (DOXYGEN_FOUND)
#         message("Doxygen need to be installed to generate the doxygen documentation")
#     endif (DOXYGEN_FOUND)
# endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

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
