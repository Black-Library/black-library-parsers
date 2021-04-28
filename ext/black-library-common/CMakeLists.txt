cmake_minimum_required(VERSION 3.5)

if(TARGET blacklibrarycommon)
    return() # if already has target, don't make a new one
endif()

project(BlackLibraryCommonProject VERSION 0.1 LANGUAGES C CXX)

set(BUILD_SHARED_LIBS ON)

add_compile_options(-Wall -Wextra -Werror)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(src)
