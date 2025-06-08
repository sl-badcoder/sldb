cmake_minimum_required(VERSION 3.28)
project(SLDB)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Define the executable and its source files
add_executable(SLDB
        src/sldb.cpp
        src/database/database.cpp
        src/database/database.h
        src/database/datastructures/indexing.cpp
        src/database/buffermanager/buffer_manager.cpp
        src/database/datastructures/indexing.h
        src/database/buffermanager/buffer_manager.h
        src/database/buffermanager/buffer_replacement.h
        src/storage/basic_frame.h
        src/storage/slotted_page.h
        src/storage/advanced_frame.h
        src/storage/slotted_page.cpp
        src/database/buffermanager/buffer_replacement.cpp
        include/File.h
)

# include google tests
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/7e2c425db2c2e024b2807bfe6d386f4ff068d0d6.zip
)

FetchContent_MakeAvailable(googletest)
# Add the include directory to the target
target_include_directories(SLDB PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(SLDB PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(SLDB PRIVATE src/)
target_include_directories(SLDB PRIVATE
        ${gtest_SOURCE_DIR}/include
)

include(GoogleTest)

# include the testing
include("${PROJECT_SOURCE_DIR}/tests/local.cmake")
