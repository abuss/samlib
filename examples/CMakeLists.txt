
macro(ADD_SAMLIB_TEST TEST_NAME TEST_SOURCE)
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  target_link_libraries(${TEST_NAME} samlib)
endmacro()

ADD_SAMLIB_TEST(example1 example1.cc)
ADD_SAMLIB_TEST(example2 example2.cc)
ADD_SAMLIB_TEST(example3 example3.cc)

ADD_SAMLIB_TEST(example4a example4a.cc)
ADD_SAMLIB_TEST(example4b example4b.cc)
ADD_SAMLIB_TEST(example4c example4c.cc)

ADD_SAMLIB_TEST(example5 example5.cc)
ADD_SAMLIB_TEST(example6 example6.cc)
ADD_SAMLIB_TEST(example7 example7.cc)
ADD_SAMLIB_TEST(example8 example8.cc)
ADD_SAMLIB_TEST(example9 example9.cc)
ADD_SAMLIB_TEST(example10 example10.cc)

ADD_SAMLIB_TEST(example11a example11a.cc)
ADD_SAMLIB_TEST(example11b example11b.cc)

ADD_SAMLIB_TEST(example12 example12.cc)

ADD_SAMLIB_TEST(example13 example13.cc)

ADD_SAMLIB_TEST(example14 example14.cc)