cmake_minimum_required(VERSION 2.8.11)

file(GLOB_RECURSE VKL_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE VKL_SRC *.c)

add_library(VKL ${VKL_HEADERS} ${VKL_SRC})

target_include_directories (VKL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}../../include)