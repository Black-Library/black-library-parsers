find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)
if (REQUIRE_PYTHON)
    message("REQUIRE_PYTHON ON")
    find_package(PythonLibs 3 REQUIRED Development)

    message(STATUS "Checking Python Deps")
    execute_process(COMMAND python3 PythonCheckDeps.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE PYTHON_DEPS_OUTPUT)
    message(STATUS "Finished Checking Python Deps")

    if(NOT PYTHON_DEPS_OUTPUT STREQUAL "")
        message(FATAL_ERROR "ERROR: Missing Python Deps: " ${PYTHON_DEPS_OUTPUT})
        return(1)
    endif()
else()
    message("REQUIRE_PYTHON OFF")
endif()

message("LibXml2 include dir: ${LIBXML2_INCLUDE_DIR}")

include(GNUInstallDirs)

add_library(blacklibraryparsers
    IndexEntryParser.cc
    LinkedListParser.cc

    Parser.cc
    ParserCommon.cc
    ParserFactory.cc
    ParserManager.cc
    ParserWorker.cc

    ParserAO3.cc
    ParserFFN.cc
    ParserRR.cc
    ParserSBF.cc
    ParserSVF.cc
    ParserXF.cc
    ParserYT.cc

    ReaderTimeGenerator.cc
    ShortTimeGenerator.cc

    NetworkAdapter.cc
    CurlAdapter.cc
    SeleniumAdapter.cc

    SeleniumAdapter.py
    )

target_link_libraries(blacklibraryparsers blacklibrarycommon ${CURL_LIBRARY} ${LIBXML2_LIBRARIES} ${PYTHON_LIBRARIES} pthread)
target_include_directories(blacklibraryparsers PUBLIC ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

install(
  TARGETS blacklibraryparsers
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(blacklibraryparsers PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")

add_executable(parser_driver parser_driver.cc)
add_executable(parser_worker_driver parser_worker_driver.cc)
add_executable(parser_manager_driver parser_manager_driver.cc)
add_executable(thread_pool_example ThreadpoolExample.cc)
add_executable(parser_tester Tester.cc)

<<<<<<< HEAD
target_link_libraries(parser_driver Parsers)
target_link_libraries(parser_manager_driver Parsers)
target_link_libraries(thread_pool_example Parsers)
target_link_libraries(parser_tester Parsers)
=======
target_link_libraries(parser_driver blacklibraryparsers)
target_link_libraries(parser_worker_driver blacklibraryparsers)
target_link_libraries(parser_manager_driver blacklibraryparsers)
target_link_libraries(thread_pool_example blacklibraryparsers)
>>>>>>> tmp2

set_target_properties(parser_driver PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")
set_target_properties(parser_worker_driver PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")
set_target_properties(parser_manager_driver PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")

message("linker path: $LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")

<<<<<<< HEAD
install(TARGETS parser_driver parser_manager_driver thread_pool_example parser_tester RUNTIME DESTINATION bin)
=======
install(TARGETS parser_driver parser_worker_driver parser_manager_driver thread_pool_example RUNTIME DESTINATION bin)
>>>>>>> tmp2
