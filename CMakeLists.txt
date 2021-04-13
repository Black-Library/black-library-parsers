cmake_minimum_required(VERSION 3.5)

project(ParsersProject VERSION 0.1 LANGUAGES C CXX)

set(BUILD_SHARED_LIBS ON)

add_compile_options(-Wall -Wextra -Werror)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(cmake/docker.cmake)

configure_file(include/ParsersConfig.h.in include/ParsersConfig.h)

add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(test)

add_parser_docker_targets()
