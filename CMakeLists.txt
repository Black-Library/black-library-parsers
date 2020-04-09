cmake_minimum_required(VERSION 3.5)

project(Parsers VERSION 0.1 LANGUAGES C CXX)

set(BUILD_SHARED_LIBS ON)

add_compile_options(-Wall -Wextra -Werror)

set(CMAKE_CPP_STANDARD 11)
set(CMAKE_CPP_STANDARD_REQUIRED True)

configure_file(include/ParsersConfig.h.in include/ParsersConfig.h)

add_subdirectory(src)
