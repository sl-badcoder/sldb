# add tests
set(TEST_CC tests/file_test.cc tests/replacement_test.cc tests/slotted_page_test.cpp)

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
        bufferlib
        project_includes
        GTest::gtest_main
)

# discover tests

gtest_discover_tests(tester)
