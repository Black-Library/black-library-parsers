cmake_minimum_required(VERSION 3.5)

project(Parsers VERSION 0.1)

set(CMAKE_CPP_STANDARD 11)
set(CMAKE_CPP_STANDARD_REQUIRED True)

configure_file(ParsersConfig.h.in ParsersConfig.h)

add_executable(Parsers Tester.cpp)

target_include_directories(Parsers PUBLIC
  "${PROJECT_BINARY_DIR}")
