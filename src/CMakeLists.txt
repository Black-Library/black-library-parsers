find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

message("LibXml2 include dir: ${LIBXML2_INCLUDE_DIR}")

include(GNUInstallDirs)

add_library(Parsers Parser.cpp ParserAO3.cpp SourceInformation.cpp)
target_link_libraries(Parsers ${CURL_LIBRARY} ${LIBXML2_LIBRARIES})
target_include_directories(Parsers PUBLIC ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

install(
  TARGETS Parsers
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_executable(Tester Tester.cpp)

target_link_libraries(Tester Parsers)

set_target_properties(Tester PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH;${CMAKE_INSTALL_PREFIX}/lib")

install(TARGETS Tester RUNTIME DESTINATION bin)
