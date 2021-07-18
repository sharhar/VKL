#include <VKL/VKLSurface.h>
#include <VKL/VKLPhysicalDevice.h>
#include <VKL/VKLInstance.h>

VKLSurface::VKLSurface(VkSurfaceKHR surface, VKLInstance* instance) {
	m_handle = VK_NULL_HANDLE;
	m_instance = instance;
	m_width = -1;
	m_height = -1;
	m_handle = surface;
}

VkSurfaceKHR VKLSurface::handle() {
	return m_handle;
}

void VKLSurface::setSize(int width, int height) {
	m_width = width;
	m_height = height;
}

void VKLSurface::destroy() {
	m_instance->vk.DestroySurfaceKHR(m_instance->handle(), m_handle, m_instance->allocator());
}

VkBool32 VKLSurface::getPhysicalDeviceSupport(VKLPhysicalDevice* physicalDevice, uint32_t queueFamilyIndex) {
	VkBool32 result = VK_FALSE;
	VK_CALL(m_instance->vk.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice->handle(), queueFamilyIndex, m_handle, &result));
	return result;
}
