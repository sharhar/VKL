cmake_minimum_required(VERSION 2.8.11)

file(GLOB_RECURSE VKL_TEST_SRC *.c)

if(APPLE)
	add_executable(
		VKL-Test 
		MACOSX_BUNDLE 
		${VKL_TEST_SRC}
	)
else()
	add_executable(VKL-Test ${VKL_TEST_SRC})
endif()

target_link_libraries (VKL-Test LINK_PUBLIC VKL)
target_link_libraries (VKL-Test glfw ${GLFW_LIBRARIES})
