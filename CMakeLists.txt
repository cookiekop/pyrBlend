cmake_minimum_required(VERSION 3.9)
project(pyrBlend)

set(CMAKE_CXX_STANDARD 11)

add_executable(pyrBlend main.cpp)

find_package( OpenCV REQUIRED )

target_link_libraries(pyrBlend ${OpenCV_LIBS})