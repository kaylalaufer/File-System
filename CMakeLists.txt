cmake_minimum_required(VERSION 3.16)
project(DiskManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/opt/homebrew/Cellar/googletest/1.15.2/include)
link_directories(/opt/homebrew/Cellar/googletest/1.15.2/lib)

add_executable(runTests test_main.cpp disk_manager.cpp)

target_link_libraries(runTests gtest gtest_main pthread)
