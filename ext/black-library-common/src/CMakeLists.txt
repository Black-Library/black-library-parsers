include(GNUInstallDirs)

add_library(blacklibrarycommon
    SourceInformation.cc
    )

target_include_directories(blacklibrarycommon PUBLIC ${PROJECT_SOURCE_DIR}/include)

install(
  TARGETS blacklibrarycommon
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(blacklibrarycommon PROPERTIES INSTALL_RPATH
    "$LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")
