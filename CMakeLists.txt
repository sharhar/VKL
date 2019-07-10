cmake_minimum_required(VERSION 2.8.11)

set(EXT_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext")

project(VKL)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(VKL_BUILD_TEST "Build the VKL-Test project" OFF)

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/VKL")

if(VKL_BUILD_TEST)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/VKL-Test")
endif()