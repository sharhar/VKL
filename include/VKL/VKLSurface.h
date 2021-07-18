#ifndef VKLSurface_h
#define VKLSurface_h

#include "VKL_base.h"

class VKLSurface {
public:
	VKLSurface(VkSurfaceKHR surface, VKLInstance* instance);

	VkSurfaceKHR handle();
	void setSize(int width, int height);
	void destroy();

	VkBool32 getPhysicalDeviceSupport(VKLPhysicalDevice* physicalDevice, uint32_t queueFamilyIndex);

	//void _vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities);
	//void _vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats);
	//void _vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes);
private:
	VkSurfaceKHR m_handle;

	VKLInstance* m_instance;
	int m_width, m_height;
};

#endif