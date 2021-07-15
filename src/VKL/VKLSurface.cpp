#include <VKL/VKLSurface.h>
#include <VKL/VKLInstance.h>

VKLSurface::VKLSurface() {
	m_surface = VK_NULL_HANDLE;
	m_width = -1;
	m_height = -1;
	m_instance = NULL;
}

VKLSurface::VKLSurface(VkSurfaceKHR surface, VKLInstance* instance) {
	m_surface = VK_NULL_HANDLE;
	m_width = -1;
	m_height = -1;
	m_instance = NULL;
	create(surface, instance);
}

void VKLSurface::create(VkSurfaceKHR surface, VKLInstance* instance) {
	m_instance = instance;

	m_vkDestroySurfaceKHR = instance->__vkDestroySurfaceKHR;
	m_vkGetPhysicalDeviceSurfaceSupportKHR = instance->__vkGetPhysicalDeviceSurfaceSupportKHR;
	m_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = instance->__vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	m_vkGetPhysicalDeviceSurfaceFormatsKHR = instance->__vkGetPhysicalDeviceSurfaceFormatsKHR;
	m_vkGetPhysicalDeviceSurfacePresentModesKHR = instance->__vkGetPhysicalDeviceSurfacePresentModesKHR;

}

void VKLSurface::setSize(int width, int height) {
	m_width = width;
	m_height = height;
}

void VKLSurface::destroy() {
	vkDestroySurfaceKHR(m_instance->allocator());
}

void VKLSurface::vkDestroySurfaceKHR(const VkAllocationCallbacks* pAllocator) {
	m_vkDestroySurfaceKHR(m_instance->handle(), m_surface, pAllocator);
}
void VKLSurface::vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkBool32* pSupported) {
	VK_CALL(m_vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, m_surface, pSupported));
}
void VKLSurface::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
	VK_CALL(m_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, pSurfaceCapabilities));
}
void VKLSurface::vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) {
	VK_CALL(m_vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, pSurfaceFormatCount, pSurfaceFormats));
}
void VKLSurface::vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) {
	VK_CALL(m_vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, pPresentModeCount, pPresentModes));
}