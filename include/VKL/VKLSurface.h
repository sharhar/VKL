#ifndef VKLSurface_h
#define VKLSurface_h

#include "VKL_base.h"

class VKLSurface {
public:
	VKLSurface();
	VKLSurface(VkSurfaceKHR surface, VKLInstance* instance);

	void create(VkSurfaceKHR surface, VKLInstance* instance);
	void setSize(int width, int height);
	void destroy();

	void vkDestroySurfaceKHR(const VkAllocationCallbacks* pAllocator);
	void vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkBool32* pSupported);
	void vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities);
	void vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats);
	void vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes);
private:
	VkSurfaceKHR m_surface;
	VKLInstance* m_instance;
	int m_width, m_height;

	PFN_vkDestroySurfaceKHR m_vkDestroySurfaceKHR;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR m_vkGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR m_vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR m_vkGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR m_vkGetPhysicalDeviceSurfacePresentModesKHR;
};

#endif