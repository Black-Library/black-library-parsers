find_package(Catch2 REQUIRED)

add_executable(parser_test parser_test.cc)
target_link_libraries(parser_test PRIVATE Catch2::Catch2 blacklibraryparsers)