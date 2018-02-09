cmake_minimum_required(VERSION 3.1)
project(image-io)

set (CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(idr-test idr_test.cpp)
target_link_libraries(idr-test boost_system boost_filesystem ${OpenCV_LIBS} pthread )

add_executable(idw-test idw_test.cpp)
target_link_libraries(idw-test boost_system boost_filesystem ${OpenCV_LIBS} pthread )