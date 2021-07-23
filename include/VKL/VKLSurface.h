#ifndef VKLSurface_h
#define VKLSurface_h

#include "VKL_base.h"

#include <vector>

class VKLSurface : public VKLObject<VkSurfaceKHR> {
public:
	VKLSurface();
	
	VKLSurface& setInstance(VKLInstance* instance);
	VKLSurface& setHandle(VkSurfaceKHR surface);
	VKLSurface& setSize(int width, int height);
	
	VkExtent2D getSize();
	
	void destroy();

	VkBool32 getPhysicalDeviceSupport(VKLPhysicalDevice* physicalDevice, uint32_t queueFamilyIndex);
	VkSurfaceCapabilitiesKHR getCapabilities(VKLPhysicalDevice* physicalDevice);
	std::vector<VkSurfaceFormatKHR> getFormats(VKLPhysicalDevice* physicalDevice);
	std::vector<VkPresentModeKHR> getPresentModes(VKLPhysicalDevice* physicalDevice);
private:
	VKLInstance* m_instance;
	VkExtent2D m_size;
	
	void _build();
};

#endif
