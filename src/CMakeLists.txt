cmake_minimum_required(VERSION 2.8.11)

file(GLOB_RECURSE VKL_HEADERS "../include/*.h")
file(GLOB_RECURSE VKL_SRC *.cpp)


add_library(VKL ${VKL_HEADERS} ${VKL_SRC})

if(MSVC)
	add_compile_definitions(VKL_SURFACE_WIN32)
elseif(APPLE)
	add_compile_definitions(VKL_SURFACE_MACOS)
endif()

target_include_directories (VKL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}../../include)
