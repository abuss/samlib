cmake_minimum_required(VERSION 3.0.0)
project(samlib VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS "1")

set(PROJECT_AUTHOR "Antal Buss")
set(PROJECT_AUTHOR_EMAIL "antal.buss@gmail.com")

message("*")
message("* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message("* Copyright (c) 2018 ${PROJECT_AUTHOR} <${PROJECT_AUTHOR_EMAIL}>")
message("*")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

# Find Open Multi-Processing (OpenMP)
find_package(OpenMP QUIET)
if(OPENMP_FOUND)
    message(STATUS "Found OpenMP")
    # In case that OMP is found we force to disable Intel TBB since
    # tiny-dnn does not support mutiple multithreading backends.
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
elseif(NOT OPENMP_FOUND)
    # In case the user sets the flag USE_OMP to ON, the CMake build-tree
    # will require to find OMP in your system. Otherwise, the user can
    # set the CMAKE_C_FLAGS and CMAKE_CXX_FLAGS by hand.
    message(FATAL_ERROR "Can't find OpenMP. Please set OpenMP_C_FLAGS & "
            "OpenMP_CXX_FLAGS")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++17 support has been enabled by default.")

SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread")

include(CTest)
enable_testing()

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
