cmake_minimum_required(VERSION 3.8.2)
project("loader" LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 -D__PROJECT=\"${PROJECT_NAME}\"")
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE CXX_SOURCE "src/*.cpp")
file(GLOB_RECURSE C_SOURCE "src/*.c")

add_library("loader" SHARED ${C_SOURCE} ${CXX_SOURCE})
target_include_directories("loader" PRIVATE "include")
target_precompile_headers("loader" PRIVATE "src/pch.h")

install(
    TARGETS "loader"
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
    FILE "loader-targets.cmake"
    NAMESPACE "loader::"
)

install(
    EXPORT loader_exports
    FILE "loader-targets.cmake"
    NAMESPACE "loader::" 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

# deps
# minhook
target_include_directories("loader" PRIVATE "vendor/minhook/include")
target_link_libraries("loader" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/minhook/lib/libMinHook-x86-v141-md.lib")

# spdlog
target_include_directories("loader" PRIVATE "vendor/spdlog/include")