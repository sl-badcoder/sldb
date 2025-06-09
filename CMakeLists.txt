cmake_minimum_required(VERSION 3.11)
project(SLDB)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wvla)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -w -O3")

# include google tests
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/7e2c425db2c2e024b2807bfe6d386f4ff068d0d6.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(SLDB src/sldb.cpp)
target_link_libraries(SLDB bufferlib project_includes)

# sources
include("${CMAKE_SOURCE_DIR}/src/local.cmake")


# Add the include directory to the target
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(project_includes INTERFACE)
target_include_directories(project_includes INTERFACE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}
)

include(GoogleTest)

# include the testing
include("${PROJECT_SOURCE_DIR}/tests/local.cmake")
