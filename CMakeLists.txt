cmake_minimum_required(VERSION 3.5)

project(Parsers VERSION 0.1 LANGUAGES C CXX)

set(BUILD_SHARED_LIBS ON)
set(CURL_LIBRARY "-lcurl")

add_compile_options(-Wall -Wextra -Werror)

set(CMAKE_CPP_STANDARD 11)
set(CMAKE_CPP_STANDARD_REQUIRED True)

find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

configure_file(include/ParsersConfig.h.in include/ParsersConfig.h)

add_subdirectory(src)
