find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

message("LibXml2 include dir: ${LIBXML2_INCLUDE_DIR}")

include(GNUInstallDirs)

add_library(Parsers Parser.cc ParserAO3.cc ParserFactory.cc ParserManager.cc SourceInformation.cc)
target_link_libraries(Parsers ${CURL_LIBRARY} ${LIBXML2_LIBRARIES} pthread)
target_include_directories(Parsers PUBLIC ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

install(
  TARGETS Parsers
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_executable(parser_driver parser_driver.cc)
add_executable(parser_manager_driver parser_manager_driver.cc)
add_executable(thread_pool_example ThreadpoolExample.cc)

target_link_libraries(parser_driver Parsers)
target_link_libraries(parser_manager_driver Parsers)
target_link_libraries(thread_pool_example Parsers)

set_target_properties(parser_driver PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH;${CMAKE_INSTALL_PREFIX}/lib")
set_target_properties(parser_manager_driver PROPERTIES INSTALL_RPATH
"$LD_LIBRARY_PATH;${CMAKE_INSTALL_PREFIX}/lib")

install(TARGETS parser_driver parser_manager_driver thread_pool_example RUNTIME DESTINATION bin)
