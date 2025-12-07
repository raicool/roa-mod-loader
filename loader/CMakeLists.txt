cmake_minimum_required(VERSION 3.8.2)
project("loader" LANGUAGES C CXX ASM)

file(GLOB_RECURSE CXX_SOURCE "src/*.cpp")
file(GLOB_RECURSE C_SOURCE "src/*.c")

file(READ ${ROA_MOD_LOADER_ROOT_DIR}/version LOADER_VERSION)
add_compile_definitions("LOADER_VERSION=\"${LOADER_VERSION}\"")
add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_library(${PROJECT_NAME} SHARED ${C_SOURCE} ${CXX_SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (ROA_MOD_LOADER_ROOT_DIR)
	message("--- ${ROA_MOD_LOADER_ROOT_DIR}")
else()
	message(FATAL_ERROR "ROA_MOD_LOADER_ROOT_DIR not found")
endif()