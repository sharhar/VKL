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


VkExtent2D VKLSurface::getSize() {
	VkExtent2D result;
	result.width = m_width;
	result.height = m_height;
	return result;
}

void VKLSurface::destroy() {
	m_instance->vk.DestroySurfaceKHR(m_instance->handle(), m_handle, m_instance->allocator());
}

VkBool32 VKLSurface::getPhysicalDeviceSupport(VKLPhysicalDevice* physicalDevice, uint32_t queueFamilyIndex) {
	VkBool32 result = VK_FALSE;
	VK_CALL(m_instance->vk.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice->handle(), queueFamilyIndex, m_handle, &result));
	return result;
}

VkSurfaceCapabilitiesKHR VKLSurface::getCapabilities(VKLPhysicalDevice* physicalDevice) {
	VkSurfaceCapabilitiesKHR result;
	m_instance->vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->handle(), m_handle, &result);
	return result;
}

std::vector<VkSurfaceFormatKHR> VKLSurface::getFormats(VKLPhysicalDevice* physicalDevice) {
	std::vector<VkSurfaceFormatKHR> result;
	uint32_t count;
	m_instance->vk.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle(), m_handle, &count, NULL);
	result.resize(count);
	m_instance->vk.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle(), m_handle, &count, result.data());
	return result;
}

std::vector<VkPresentModeKHR> VKLSurface::getPresentModes(VKLPhysicalDevice* physicalDevice) {
	std::vector<VkPresentModeKHR> result;
	uint32_t count;
	m_instance->vk.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle(), m_handle, &count, NULL);
	result.reserve(count);
	m_instance->vk.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle(), m_handle, &count, result.data());
	return result;
}
