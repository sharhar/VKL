#ifndef VKLPhysicalDevice_h
#define VKLPhysicalDevice_h

#include "VKL_base.h"

#include <vector>

class VKLPhysicalDevice : public VKLHandle<VkPhysicalDevice> {
public:
	VKLPhysicalDevice(VkPhysicalDevice physicalDevice, VKLInstance* instance);

	VkPhysicalDeviceFeatures getFeatures();
	VkFormatProperties getFormatProperties(VkFormat format);
	VkImageFormatProperties getImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags);
	VkPhysicalDeviceProperties getProperties();
	const std::vector<VkQueueFamilyProperties>& getQueueFamilyProperties();
	VkPhysicalDeviceMemoryProperties getMemoryProperties();
	std::vector<VkSparseImageFormatProperties> getSparseImageProperties(VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling);
	std::vector<VkExtensionProperties>& getExtensions();
	
	VkBool32 getSurfaceSupport(VkSurfaceKHR surface, uint32_t queueFamilyIndex);
	VkSurfaceCapabilitiesKHR getSurfaceCapabilities(VkSurfaceKHR surface);
	std::vector<VkSurfaceFormatKHR> getSurfaceFormats(VkSurfaceKHR surface);
	std::vector<VkPresentModeKHR> getSurfacePresentModes(VkSurfaceKHR surface);
private:
	VKLInstance* m_instance;

	VkPhysicalDeviceFeatures m_features;
	VkPhysicalDeviceProperties m_properties;
	std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	std::vector<VkExtensionProperties> m_extensions;
	VkPhysicalDeviceMemoryProperties m_memoryProperties;
};

#endif
