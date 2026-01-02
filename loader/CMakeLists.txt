cmake_minimum_required(VERSION 3.8.2)
project("loader" LANGUAGES C CXX ASM)

set(CXXFLAGS "${CXXFLAGS} /permissive-")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /permissive-")

if (ROA_MOD_LOADER_ROOT_DIR)
	message("--- ${ROA_MOD_LOADER_ROOT_DIR}")
else()
	message(FATAL_ERROR "ROA_MOD_LOADER_ROOT_DIR not found")
endif()

file(GLOB_RECURSE CXX_SOURCE "src/*.cpp")
file(GLOB_RECURSE C_SOURCE "src/*.c")
file(GLOB IMGUI_SOURCE 
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/imgui.cpp"
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/imgui_widgets.cpp"
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/imgui_tables.cpp"
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/imgui_draw.cpp"
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/imgui_demo.cpp"
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/backends/imgui_impl_win32.cpp"
    "${ROA_MOD_LOADER_ROOT_DIR}/vendor/imgui/backends/imgui_impl_dx11.cpp"
)

file(READ ${ROA_MOD_LOADER_ROOT_DIR}/version LOADER_VERSION)
add_compile_definitions("LOADER_VERSION=\"${LOADER_VERSION}\"")
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

add_subdirectory(UGMMS)

add_library(${PROJECT_NAME} SHARED ${IMGUI_SOURCE} ${C_SOURCE} ${CXX_SOURCE} ${UGMMS_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (EXISTS ${UGMMS_INCLUDE_DIR})
	target_include_directories(${PROJECT_NAME} PUBLIC "${UGMMS_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "UGMMS include dir not set")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${UGMMS_DETOURS_LIB})