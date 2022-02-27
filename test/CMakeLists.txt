include_directories(${PROJECT_SOURCE_DIR}/ext/catch2)

set(SOURCES_TESTS
    parser_common_test.cc
    parser_test.cc
    )

string( REPLACE ".cc" "" BASENAMES_TESTS "${SOURCES_TESTS}" )
set( TARGETS_TESTS ${BASENAMES_TESTS} )
    
foreach( name ${TARGETS_TESTS} )
    add_executable( ${name} ${name}.cc )
endforeach()

foreach( name ${TARGETS_TESTS} )
    target_link_libraries( ${name} Catch2::Catch2WithMain blacklibraryparsers)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${name} PROPERTY CXX_EXTENSIONS OFF)
endforeach()

foreach( name ${TARGETS_LIST} )
    install(TARGETS
        ${name}
        RUNTIME DESTINATION bin
    )
endforeach()
