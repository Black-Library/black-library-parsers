find_package(CURL REQUIRED)
find_package(LIBXML2 REQUIRED)

add_library(Parsers Parser.cpp ParserAO3.cpp)
target_link_libraries(Parsers ${CURL_LIBRARY} ${LIBXML2_LIBRARY})
target_include_directories(Parsers PUBLIC ${CURL_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

install(TARGETS Parsers LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_executable(Tester Tester.cpp)

target_link_libraries(Tester Parsers)

install(TARGETS Tester RUNTIME DESTINATION bin)