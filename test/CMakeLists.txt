
macro(ADD_SAMLIB_TEST TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} samlib)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

ADD_SAMLIB_TEST(test1 test1.cc)

