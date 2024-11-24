cmake_minimum_required(VERSION 3.10)
project(FinalProject)

# Set the C++ standard to C++14 (or C++17 if preferred)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Google Test
include_directories(/opt/homebrew/Cellar/googletest/1.15.2/include)
link_directories(/opt/homebrew/Cellar/googletest/1.15.2/lib)

add_definitions(-DTESTING)


# Add source files
add_executable(runTests test_main.cpp disk_manager.cpp)

# Link Google Test and pthread (required by GTest)
target_link_libraries(runTests gtest gtest_main pthread)
