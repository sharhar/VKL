#ifndef VKLPhysicalDevice_h
#define VKLPhysicalDevice_h

#include "VKL_base.h"

#include <vector>

class VKLPhysicalDevice {
public:
	VKLPhysicalDevice(VkPhysicalDevice physicalDevice, VKLInstance* instance);

	VkPhysicalDevice handle();

	VkPhysicalDeviceFeatures getFeatures();
	VkFormatProperties getFormatProperties(VkFormat format);
	VkImageFormatProperties getImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags);
	VkPhysicalDeviceProperties getProperties();
	const std::vector<VkQueueFamilyProperties>& getQueueFamilyProperties();
	VkPhysicalDeviceMemoryProperties getMemoryProperties();
	std::vector<VkSparseImageFormatProperties> getSparseImageProperties(VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling);
	std::vector<VkExtensionProperties>& getExtensions();
private:
	VkPhysicalDevice m_handle;
	VKLInstance* m_instance;

	VkPhysicalDeviceFeatures m_features;
	VkPhysicalDeviceProperties m_properties;
	std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	std::vector<VkExtensionProperties> m_extensions;
	VkPhysicalDeviceMemoryProperties m_memoryProperties;
};

#endif
