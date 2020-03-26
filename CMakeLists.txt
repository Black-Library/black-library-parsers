cmake_minimum_required(VERSION 3.5)

project(Parsers VERSION 0.1)

set(CMAKE_CPP_STANDARD 11)
set(CMAKE_CPP_STANDARD_REQUIRED True)

configure_file(include/ParsersConfig.h.in include/ParsersConfig.h)

include_directories(include)
include_directories(src)

file(GLOB SOURCES "src/*.cpp")

add_executable(Parsers src/Tester.cpp)
