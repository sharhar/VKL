cmake_minimum_required(VERSION 2.8.11)

file(GLOB_RECURSE VKL_TEST_SRC *.cpp)

if(MSVC)
	add_executable(VKL-Test ${VKL_TEST_SRC})
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../res" DESTINATION "${CMAKE_BINARY_DIR}/src/VKL-Test")
else()
	add_executable(VKL-Test ${VKL_TEST_SRC})
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

target_link_libraries (VKL-Test LINK_PUBLIC VKL)
target_link_libraries (VKL-Test glfw ${GLFW_LIBRARIES})
