#ifdef VKL_SURFACE_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <VKL/VKL.h>

#ifdef VKL_SURFACE_GLFW
typedef VkResult (*PFN_glfwCreateWindowSurface)(VkInstance, void*, const VkAllocationCallbacks*, VkSurfaceKHR*);
#endif

VKLSurface::VKLSurface() : VKLCreator<VKLSurfaceCreateInfo>("VKLSurface") {
	m_instance = NULL;
}

VKLSurface::VKLSurface(const VKLSurfaceCreateInfo& createInfo) : VKLCreator<VKLSurfaceCreateInfo>("VKLSurface") {
	m_instance = NULL;
	create(createInfo);
}

void VKLSurface::_create(const VKLSurfaceCreateInfo& createInfo) {
	m_instance = createInfo.m_instance;

	void** ptrs;

	switch (createInfo.m_type) {
	case VKL_SURFACE_CREATE_INFO_TYPE_HANDLE:
		m_handle = *(VkSurfaceKHR*)createInfo.m_createInfoPtr;
		break;
	case VKL_SURFACE_CREATE_INFO_TYPE_GLFW:
#ifdef VKL_SURFACE_GLFW
		ptrs = (void**)createInfo.m_createInfoPtr;

		((PFN_glfwCreateWindowSurface)ptrs[0])(m_instance->handle(), ptrs[1], 
												m_instance->allocationCallbacks(), &m_handle);
#endif
		break;
	case VKL_SURFACE_CREATE_INFO_TYPE_WIN32:
#ifdef VKL_SURFACE_WIN32
		((PFN_vkCreateWin32SurfaceKHR)m_instance->vk.CreateWin32SurfaceKHR)(m_instance->handle(), 
													(VkWin32SurfaceCreateInfoKHR*)createInfo.m_createInfoPtr, 
													m_instance->allocationCallbacks(), &m_handle);
#endif
		break;
	default:
		break;
	}
}

void VKLSurface::_destroy() {
	m_instance->vk.DestroySurfaceKHR(m_instance->handle(), m_handle, m_instance->allocationCallbacks());
}

VKLSurfaceCreateInfo::VKLSurfaceCreateInfo() {
	m_createInfoPtr = NULL;
	m_type = VKL_SURFACE_CREATE_INFO_TYPE_UNDEFINED;
	m_instance = NULL;
}

VKLSurfaceCreateInfoHandle::VKLSurfaceCreateInfoHandle() {
	m_handle = VK_NULL_HANDLE;
	m_type = VKL_SURFACE_CREATE_INFO_TYPE_HANDLE;
}

VKLSurfaceCreateInfoHandle& VKLSurfaceCreateInfoHandle::handle(VkSurfaceKHR handle) {
	m_handle = handle;

	m_createInfoPtr = &m_handle;

	return (VKLSurfaceCreateInfoHandle&)invalidate();
}

VKLSurfaceCreateInfoHandle& VKLSurfaceCreateInfoHandle::instance(const VKLInstance* instance) {
	m_instance = instance;

	return (VKLSurfaceCreateInfoHandle&)invalidate();
}

bool VKLSurfaceCreateInfoHandle::_validate() {
	if (m_instance == NULL) {
		printf("VKL Validation Error: VKLSurfaceCreateInfoHandle::instance was not set!\n");
		return false;
	}

	if (m_handle == VK_NULL_HANDLE) {
		printf("VKL Validation Error: VKLSurfaceCreateInfoHandle::handle was not set!\n");
		return false;
	}

	return true;
}

#ifdef VKL_SURFACE_WIN32

VKLSurfaceCreateInfoWin32::VKLSurfaceCreateInfoWin32() {
	m_createInfoPtr = malloc(sizeof(VkWin32SurfaceCreateInfoKHR));
	m_type = VKL_SURFACE_CREATE_INFO_TYPE_WIN32;

	VkWin32SurfaceCreateInfoKHR* createInfo = (VkWin32SurfaceCreateInfoKHR*)m_createInfoPtr;

	createInfo->sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo->pNext = NULL;
	createInfo->flags = 0;
	createInfo->hinstance = GetModuleHandle(NULL);
	createInfo->hwnd = NULL;
}

VKLSurfaceCreateInfoWin32::~VKLSurfaceCreateInfoWin32() {
	free(m_createInfoPtr);
}

VKLSurfaceCreateInfoWin32& VKLSurfaceCreateInfoWin32::instance(const VKLInstance* instance) {
	m_instance = instance;

	return (VKLSurfaceCreateInfoWin32&)invalidate();
}

VKLSurfaceCreateInfoWin32& VKLSurfaceCreateInfoWin32::hinstance(void* hinstance) {
	VkWin32SurfaceCreateInfoKHR* createInfo = (VkWin32SurfaceCreateInfoKHR*)m_createInfoPtr;

	createInfo->hinstance = (HINSTANCE)hinstance;

	return (VKLSurfaceCreateInfoWin32&)invalidate();
}

VKLSurfaceCreateInfoWin32& VKLSurfaceCreateInfoWin32::hwnd(void* hwnd) {
	VkWin32SurfaceCreateInfoKHR* createInfo = (VkWin32SurfaceCreateInfoKHR*)m_createInfoPtr;

	createInfo->hwnd = (HWND)hwnd;

	return (VKLSurfaceCreateInfoWin32&)invalidate();
}

bool VKLSurfaceCreateInfoWin32::_validate() {
	if (m_instance == NULL) {
		printf("VKL Validation Error: VKLSurfaceCreateInfoWin32::instance was not set!\n");
		return false;
	}

	VkWin32SurfaceCreateInfoKHR* createInfo = (VkWin32SurfaceCreateInfoKHR*)m_createInfoPtr;

	if (createInfo->hwnd == NULL) {
		printf("VKL Validation Error: VKLSurfaceCreateInfoWin32::hwnd was not set!\n");
		return false;
	}

	return true;
}

#endif

#ifdef VKL_SURFACE_GLFW

VKLSurfaceCreateInfoGLFW::VKLSurfaceCreateInfoGLFW() {
	m_ptrs[0] = NULL;
	m_ptrs[1] = NULL;

	m_createInfoPtr = m_ptrs;

	m_type = VKL_SURFACE_CREATE_INFO_TYPE_GLFW;
}

VKLSurfaceCreateInfoGLFW& VKLSurfaceCreateInfoGLFW::instance(const VKLInstance* instance) {
	m_instance = instance;

	return (VKLSurfaceCreateInfoGLFW&)invalidate();
}

VKLSurfaceCreateInfoGLFW& VKLSurfaceCreateInfoGLFW::window(void* window) {
	m_ptrs[1] = window;

	return (VKLSurfaceCreateInfoGLFW&)invalidate();
}

VKLSurfaceCreateInfoGLFW& VKLSurfaceCreateInfoGLFW::createSurfaceFunc(void* func) {
	m_ptrs[0] = func;

	return (VKLSurfaceCreateInfoGLFW&)invalidate();
}
	
bool VKLSurfaceCreateInfoGLFW::_validate() {
	if (m_ptrs[0] == NULL) {
		printf("VKL Validation Error: VKLSurfaceCreateInfoGLFW::createSurfaceFunc was not set!\n");
		return false;
	}

	if (m_ptrs[1] == NULL) {
		printf("VKL Validation Error: VKLSurfaceCreateInfoGLFW::window was not set!\n");
		return false ;
	}

	return true;
}

#endif