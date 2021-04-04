find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

message("LibXml2 include dir: ${LIBXML2_INCLUDE_DIR}")

include(GNUInstallDirs)

add_library(blacklibraryparsers
    Parser.cc
    ParserCommon.cc
    ParserFactory.cc
    ParserManager.cc
    ParserWorker.cc
    SourceInformation.cc

    ParserAO3.cc
    # ParserFFN.cc
    ParserRR.cc
    )

target_link_libraries(blacklibraryparsers ${CURL_LIBRARY} ${LIBXML2_LIBRARIES} pthread)
target_include_directories(blacklibraryparsers PUBLIC ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

install(
  TARGETS blacklibraryparsers
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_executable(parser_driver parser_driver.cc)
add_executable(parser_worker_driver parser_worker_driver.cc)
add_executable(parser_manager_driver parser_manager_driver.cc)
add_executable(thread_pool_example ThreadpoolExample.cc)

target_link_libraries(parser_driver blacklibraryparsers)
target_link_libraries(parser_worker_driver blacklibraryparsers)
target_link_libraries(parser_manager_driver blacklibraryparsers)
target_link_libraries(thread_pool_example blacklibraryparsers)

set_target_properties(parser_driver PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH;${CMAKE_INSTALL_PREFIX}/lib")
set_target_properties(parser_worker_driver PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH;${CMAKE_INSTALL_PREFIX}/lib")
set_target_properties(parser_manager_driver PROPERTIES INSTALL_RPATH
"$LD_LIBRARY_PATH;${CMAKE_INSTALL_PREFIX}/lib")

install(TARGETS parser_driver parser_worker_driver parser_manager_driver thread_pool_example RUNTIME DESTINATION bin)
