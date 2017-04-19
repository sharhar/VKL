#pragma once

#include <VKL/VKL.h>

#ifdef VKL_USE_WSI_WIN32
int vklCreateWin32Surface(VKLInstance* instance, VKLSurface** pSurface, HWND hWnd) {
	VKLSurface* surface = malloc_c(sizeof(VKLSurface));
	
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
	memset(&surfaceCreateInfo, 0, sizeof(VkWin32SurfaceCreateInfoKHR));
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	surfaceCreateInfo.hwnd = hWnd;

	PFN_vkCreateWin32SurfaceKHR pfnCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkCreateWin32SurfaceKHR");

	VLKCheck(pfnCreateWin32SurfaceKHR(instance->instance, &surfaceCreateInfo, instance->allocator, &surface->surface),
		"Failed to create Win32 surface");

	RECT rect;
	if (GetWindowRect(hWnd, &rect)) {
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		surface->width = (uint32_t)width;
		surface->height = (uint32_t)height;
	} else {
		return -1;
	}

	*pSurface = surface;

	return 0;
}
#else
int vklCreateGLFWSurface(VKLInstance* instance, VKLSurface** pSurface, GLFWwindow* window) {
	VKLSurface* surface = malloc_c(sizeof(VKLSurface));
	glfwCreateWindowSurface(instance->instance, window, instance->allocator, &surface->surface);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	surface->width = (uint32_t)width;
	surface->height = (uint32_t)height;

	*pSurface = surface;

	return 0;
}
#endif

