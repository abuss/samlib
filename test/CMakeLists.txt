set(samlib_SRC_DIR ${PROJECT_SRC_DIR})

# add_executable(test1 test1.cc)
# target_include_directories(test1 PRIVATE ${samlib_SRC_DIR})

macro(ADD_SAMLIB_TEST TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME} PRIVATE ${samlib_SRC_DIR})
    target_link_libraries(${TEST_NAME} PRIVATE ${COMMON_LINK_LIBS})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

ADD_SAMLIB_TEST(test1 test1.cc)
ADD_SAMLIB_TEST(test2 test2.cc)
ADD_SAMLIB_TEST(test3 test3.cc)
ADD_SAMLIB_TEST(test_ports test_ports.cc)
