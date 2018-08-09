cmake_minimum_required(VERSION 3.0.0)
project(samlib VERSION 0.1.0)


set(PROJECT_AUTHOR "Antal Buss")
set(PROJECT_AUTHOR_EMAIL "antal.buss@gmail.com")

message("*")
message("* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message("* Copyright (c) 2018 ${PROJECT_AUTHOR} <${PROJECT_AUTHOR_EMAIL}>")
message("*")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)

include(CTest)
enable_testing()

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
