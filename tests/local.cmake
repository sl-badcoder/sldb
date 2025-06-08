# add tests
set(TEST_CC tests/file_test.cc tests/replacement_test.cc)

# add tester executable
enable_testing()

add_executable(
        tester
        tests/tester.cpp
        ${TEST_CC}
)

# link gtest library

target_link_libraries(
        tester
        GTest::gtest_main
)

# discover tests

gtest_discover_tests(tester)
