#ifndef VKLSurface_h
#define VKLSurface_h

#include "VKL_base.h"

#include <vector>

class VKLSurface {
public:
	VKLSurface(VkSurfaceKHR surface, VKLInstance* instance);

	VkSurfaceKHR handle();
	void setSize(int width, int height);
	VkExtent2D getSize();
	void destroy();

	VkBool32 getPhysicalDeviceSupport(VKLPhysicalDevice* physicalDevice, uint32_t queueFamilyIndex);
	VkSurfaceCapabilitiesKHR getCapabilities(VKLPhysicalDevice* physicalDevice);
	std::vector<VkSurfaceFormatKHR> getFormats(VKLPhysicalDevice* physicalDevice);
	std::vector<VkPresentModeKHR> getPresentModes(VKLPhysicalDevice* physicalDevice);

	//void _vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities);
	//void _vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats);
	//void _vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes);
private:
	VkSurfaceKHR m_handle;

	VKLInstance* m_instance;
	int m_width, m_height;
};

#endif