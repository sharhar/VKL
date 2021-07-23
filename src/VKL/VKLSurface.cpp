#include <VKL/VKLSurface.h>
#include <VKL/VKLPhysicalDevice.h>
#include <VKL/VKLInstance.h>

VKLSurface::VKLSurface() {
	m_handle = VK_NULL_HANDLE;
	m_instance = NULL;
	m_size.width = 0;
	m_size.height = 0;
	m_handle = VK_NULL_HANDLE;
}

VKLSurface& VKLSurface::setInstance(VKLInstance* instance) {
	m_instance = instance;
	
	return *this;
}

VKLSurface& VKLSurface::setHandle(VkSurfaceKHR surface) {
	m_handle = surface;
	
	return *this;
}

VKLSurface& VKLSurface::setSize(int width, int height) {
	m_size.width = width;
	m_size.height = height;
	
	return *this;
}

VkExtent2D VKLSurface::getSize() {
	return m_size;
}

void VKLSurface::destroy() {
	m_instance->vk.DestroySurfaceKHR(m_instance->handle(), m_handle, m_instance->allocationCallbacks());
}

void VKLSurface::_build() {
	
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
