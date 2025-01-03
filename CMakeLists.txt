cmake_minimum_required(VERSION 3.10)
project(api_gateway_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gateway_test
    gateway.cpp
    test.cpp)

target_link_libraries(gateway_test pthread)