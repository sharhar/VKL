cmake_minimum_required(VERSION 2.8.11)

file(GLOB_RECURSE VKL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE VKL_SRC *.cpp)

if(MSVC)
	add_library(VKL ${VKL_HEADERS} ${VKL_SRC})
	add_compile_definitions(VKL_SURFACE_WIN32)
	add_compile_definitions(VKL_SURFACE_GLFW)
	add_compile_definitions(VKL_GLFW_STATIC)
else()
	add_library(VKL ${VKL_HEADERS} ${VKL_SRC})
endif()

target_include_directories (VKL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}../../include)