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
        tests/tester.hpp
)

# Add the include directory to the target
target_include_directories(SLDB PRIVATE include)
target_include_directories(SLDB PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(SLDB PRIVATE src/)