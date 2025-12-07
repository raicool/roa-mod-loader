cmake_minimum_required(VERSION 3.8.2)
project("roa-mod-loader" LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 -D__PROJECT=\"${PROJECT_NAME}\"")
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_library("roa-mod-loader" INTERFACE)

install(
    TARGETS "roa-mod-loader"
    EXPORT loader_exports
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT includes
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT library
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT library
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/loader/loader.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/loader" COMPONENT dev
)

export(
    EXPORT loader_exports
    FILE "roa-mod-loader-targets.cmake"
    NAMESPACE "roa-mod-loader::"
)

install(
    EXPORT loader_exports
    FILE "roa-mod-loader.cmake"
    NAMESPACE "roa-mod-loader::" 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

if (EXISTS ${CMAKE_INSTALL_BINDIR})
    set(ROA_MOD_LOADER_BIN_PATH ${CMAKE_INSTALL_BINDIR})
endif()

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(loader)
    target_link_libraries("roa-mod-loader" INTERFACE loader)
elseif (EXISTS ${ROA_MOD_LOADER_BIN_PATH})
    target_link_libraries("roa-mod-loader" INTERFACE ${ROA_MOD_LOADER_BIN_PATH})
else()
    message(FATAL_ERROR "could not find binary for roa-mod-loader!")
endif()