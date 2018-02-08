cmake_minimum_required(VERSION 3.1)
project(idr-test)

set (CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(idr-test test.cpp)
target_link_libraries(idr-test boost_system boost_filesystem ${OpenCV_LIBS} pthread )