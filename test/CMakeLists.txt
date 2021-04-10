include_directories(${PROJECT_SOURCE_DIR}/ext/catch2)
add_executable(parser_test parser_test.cc)
target_link_libraries(parser_test PRIVATE Catch2::Catch2WithMain blacklibraryparsers)