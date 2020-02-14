
include(ExternalProject)

message("which queue: ${QUEUE_IMPLEMENTATION}")

if ("${QUEUE_IMPLEMENTATION}" STREQUAL "concurrentqueue")
  set(GIT_REPO "https://github.com/cameron314/concurrentqueue.git")
endif()

if ("${QUEUE_IMPLEMENTATION}" STREQUAL "atomic_queue")
  set(GIT_REPO "https://github.com/max0x7ba/atomic_queue.git")
endif()

if ("${QUEUE_IMPLEMENTATION}" STREQUAL "MPMCQueue")
  set(GIT_REPO "https://github.com/rigtorp/MPMCQueue.git")
endif()

ExternalProject_Add(
  ${QUEUE_IMPLEMENTATION}
  GIT_REPOSITORY ${GIT_REPO}
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${QUEUE_IMPLEMENTATION}"
  
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  )

target_include_directories(samlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${QUEUE_IMPLEMENTATION})

# Adding support for jthreads (coming in c++20)
ExternalProject_Add(
  jthread
  GIT_REPOSITORY https://github.com/josuttis/jthread.git
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/jthread"
  
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  )

target_include_directories(samlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/jthread/source)
