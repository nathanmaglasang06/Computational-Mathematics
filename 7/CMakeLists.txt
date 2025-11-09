cmake_minimum_required(VERSION 4.1)
project(Lab7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS  OFF)

find_package(Boost REQUIRED)

add_library(llllib llllib.cpp)
target_link_libraries(llllib PUBLIC Boost::boost)

add_executable(Task1 "main.cpp")
target_link_libraries(Task1 PRIVATE llllib)

