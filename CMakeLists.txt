cmake_minimum_required(VERSION 3.13)
project("roa-mod-loader" LANGUAGES C CXX ASM)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 -D__PROJECT=\"${PROJECT_NAME}\"")
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_library("roa-mod-loader" INTERFACE)

if (EXISTS ${CMAKE_INSTALL_BINDIR})
    set(ROA_MOD_LOADER_BIN_PATH ${CMAKE_INSTALL_BINDIR})
endif()

set(ROA_MOD_LOADER_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(loader)
target_link_libraries("roa-mod-loader" INTERFACE loader)

# vendors
# minhook
target_include_directories(loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/minhook/include")
target_link_libraries(loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/minhook/lib/libMinHook-x86-v141-md.lib")

# spdlog
target_include_directories(loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")

# cpp-httlpib
target_include_directories(loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/cpp-httplib")

# yaml-cpp
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendor/yaml-cpp")
target_link_libraries(loader PRIVATE yaml-cpp::yaml-cpp)

# imgui
target_include_directories(loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui")
target_include_directories(loader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends")