add_executable(Tester Tester.cpp)

target_link_libraries(Tester libcurl libxml)
include_directories(Tester ${CMAKE_SOURCE_DIR}/include)

install(TARGETS Tester RUNTIME DESTINATION bin)