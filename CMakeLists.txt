cmake_minimum_required(VERSION 3.16)
project(pds_lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(pds_lab4 main.cpp Jobs.cpp Jobs.h Row.cpp Row.h)