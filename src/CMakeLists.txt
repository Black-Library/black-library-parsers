find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

message("lib xml dirs: ${LIBXML2_INCLUDE_DIRS}")

include(GNUInstallDirs)

add_library(Parsers Parser.cpp ParserAO3.cpp SourceInformation.cpp)
target_link_libraries(Parsers ${CURL_LIBRARY} ${LIBXML2_LIBRARIES})
target_include_directories(Parsers PUBLIC ${CURL_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

install(
  TARGETS Parsers
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_executable(Tester Tester.cpp)

target_link_libraries(Tester Parsers)

install(TARGETS Tester RUNTIME DESTINATION bin)
