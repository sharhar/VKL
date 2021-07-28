#include <VKL/VKL.h>

VKLPhysicalDevice::VKLPhysicalDevice(VkPhysicalDevice physicalDevice, const VKLInstance* instance) {
	m_handle = physicalDevice;
	m_instance = instance;

	m_instance->vk.GetPhysicalDeviceFeatures(m_handle, &m_features);
	m_instance->vk.GetPhysicalDeviceProperties(m_handle, &m_properties);
	m_instance->vk.GetPhysicalDeviceMemoryProperties(m_handle, &m_memoryProperties);

	VkQueueFamilyProperties* queueFamilyProperties = NULL;
	uint32_t queueFamilyPropertyCount = 0;

	m_instance->vk.GetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyPropertyCount, NULL);
	queueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertyCount);
	m_instance->vk.GetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyPropertyCount, queueFamilyProperties);

	for (int i = 0; i < queueFamilyPropertyCount; i++) {
		m_queueFamilyProperties.push_back(queueFamilyProperties[i]);
	}

	free(queueFamilyProperties);
	
	uint32_t extensionCount = 0;
	instance->vk.EnumerateDeviceExtensionProperties(m_handle, NULL, &extensionCount, NULL);
	m_extensions.resize(extensionCount);
	instance->vk.EnumerateDeviceExtensionProperties(m_handle, NULL, &extensionCount, m_extensions.data());
}

const VKLInstance* VKLPhysicalDevice::instance() const {
	return m_instance;
}

VkPhysicalDeviceFeatures VKLPhysicalDevice::getFeatures() const {
	return m_features;
}

VkFormatProperties VKLPhysicalDevice::getFormatProperties(VkFormat format) const {
	VkFormatProperties result;
	m_instance->vk.GetPhysicalDeviceFormatProperties(m_handle, format, &result);
	return result;
}

VkImageFormatProperties VKLPhysicalDevice::getImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags) const {
	VkImageFormatProperties result;
	VK_CALL(m_instance->vk.GetPhysicalDeviceImageFormatProperties(m_handle, format, type, tiling, usage, flags, &result));
	return result;
}

VkPhysicalDeviceProperties VKLPhysicalDevice::getProperties() const {
	return m_properties;
}

const std::vector<VkQueueFamilyProperties>& VKLPhysicalDevice::getQueueFamilyProperties() const {
	return m_queueFamilyProperties;
}

VkPhysicalDeviceMemoryProperties VKLPhysicalDevice::getMemoryProperties() const {
	return m_memoryProperties;
}

const std::vector<VkExtensionProperties>& VKLPhysicalDevice::getExtensions() const {
	return m_extensions;
}

VkBool32 VKLPhysicalDevice::getSurfaceSupport(VkSurfaceKHR surface, uint32_t queueFamilyIndex) const {
	VkBool32 result = VK_FALSE;
	VK_CALL(m_instance->vk.GetPhysicalDeviceSurfaceSupportKHR(m_handle, queueFamilyIndex, surface, &result));
	return result;
}

VkSurfaceCapabilitiesKHR VKLPhysicalDevice::getSurfaceCapabilities(VkSurfaceKHR surface) const {
	VkSurfaceCapabilitiesKHR result;
	m_instance->vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(m_handle, surface, &result);
	return result;
}

std::vector<VkSurfaceFormatKHR> VKLPhysicalDevice::getSurfaceFormats(VkSurfaceKHR surface) const {
	std::vector<VkSurfaceFormatKHR> result;
	uint32_t count;
	m_instance->vk.GetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, NULL);
	result.resize(count);
	m_instance->vk.GetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, result.data());
	return result;
}

std::vector<VkPresentModeKHR> VKLPhysicalDevice::getSurfacePresentModes(VkSurfaceKHR surface) const {
	std::vector<VkPresentModeKHR> result;
	uint32_t count;
	m_instance->vk.GetPhysicalDeviceSurfacePresentModesKHR(m_handle, surface, &count, NULL);
	result.resize(count);
	m_instance->vk.GetPhysicalDeviceSurfacePresentModesKHR(m_handle, surface, &count, result.data());
	return result;
}
