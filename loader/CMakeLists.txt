project("loader" LANGUAGES C CXX ASM)

file(GLOB_RECURSE CXX_SOURCE "src/*.cpp")
file(GLOB_RECURSE C_SOURCE "src/*.c")

add_library("loader" SHARED ${C_SOURCE} ${CXX_SOURCE})

target_include_directories("roa-mod-loader" PRIVATE "include")
target_precompile_headers("roa-mod-loader" PRIVATE "src/pch.h")

# deps
# minhook
target_include_directories("roa-mod-loader" PRIVATE "vendor/minhook/include")
target_link_libraries("roa-mod-loader" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/minhook/lib/libMinHook-x86-v141-md.lib")

# spdlog
target_include_directories("roa-mod-loader" PRIVATE "vendor/spdlog/include")