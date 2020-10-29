cmake_minimum_required(VERSION 3.0.0)
project(TinyWheel VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB TestSource
    "${PROJECT_SOURCE_DIR}/gtest/*.h"
    "${PROJECT_SOURCE_DIR}/gtest/*.cpp"
)

add_executable(mytest ${TestSource})
target_link_libraries(mytest ${GTEST_BOTH_LIBRARIES})
target_link_libraries(mytest ${CMAKE_THREAD_LIBS_INIT})
add_test(Test mytest)
enable_testing()