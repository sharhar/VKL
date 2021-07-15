#ifndef VKLInstance_h
#define VKLInstance_h

#include "VKL_base.h"

#include <vector>

typedef struct VKLInstanceOptions {

	VKLInstanceOptions();

	void setAllocator(VkAllocationCallbacks* allocator);
	void addExtension(char* extension);
	void addExtensions(char** extensions, uint32_t extensionCount);
	void addExtensions(std::vector<char*> extensions);
	void setDebug(VkBool32 debug);

	VkAllocationCallbacks* m_allocator;
	std::vector<char*> m_extensions;
	VkBool32 m_debug;

} VKLInstanceOptions;

class VKLInstance {
private:
	VkInstance m_instance;

	VkAllocationCallbacks* m_allocator;

	VkDebugReportCallbackEXT m_debugCallback;

	std::vector<char*> m_layers;
	std::vector<char*> m_extensions;

	VkBool32 m_debug;
public:
	VKLInstance();
	VKLInstance(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options);

	void create(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options);
	VkAllocationCallbacks* allocator();
	VkInstance handle();
	void destroy();

	void vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance);
	void vkDestroyInstance(const VkAllocationCallbacks* pAllocator);
	void vkEnumeratePhysicalDevices(uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices);
	void vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures);
	void vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties);
	void vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties);
	void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties);
	void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties);
	void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties);
	PFN_vkVoidFunction vkGetInstanceProcAddr(const char* pName);
	void vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice);
	void vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator);
	void vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties);
	void vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties);
	void vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties);
	void vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties);
	
	void vkCreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	void vkDestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);
	void vkDebugReportMessageEXT(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage);


	PFN_vkDestroySurfaceKHR __vkDestroySurfaceKHR;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR __vkGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR __vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR __vkGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR __vkGetPhysicalDeviceSurfacePresentModesKHR;
private:																																														
	PFN_vkCreateInstance m_vkCreateInstance;
	PFN_vkDestroyInstance m_vkDestroyInstance;
	PFN_vkEnumeratePhysicalDevices m_vkEnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceFeatures m_vkGetPhysicalDeviceFeatures;
	PFN_vkGetPhysicalDeviceFormatProperties m_vkGetPhysicalDeviceFormatProperties;
	PFN_vkGetPhysicalDeviceImageFormatProperties m_vkGetPhysicalDeviceImageFormatProperties;
	PFN_vkGetPhysicalDeviceProperties m_vkGetPhysicalDeviceProperties;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties m_vkGetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceMemoryProperties m_vkGetPhysicalDeviceMemoryProperties;
	PFN_vkGetInstanceProcAddr m_vkGetInstanceProcAddr;
	PFN_vkCreateDevice m_vkCreateDevice;
	PFN_vkDestroyDevice m_vkDestroyDevice;
	PFN_vkEnumerateInstanceExtensionProperties m_vkEnumerateInstanceExtensionProperties;
	PFN_vkEnumerateDeviceExtensionProperties m_vkEnumerateDeviceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties m_vkEnumerateInstanceLayerProperties;
	PFN_vkEnumerateDeviceLayerProperties m_vkEnumerateDeviceLayerProperties;
	
	PFN_vkCreateDebugReportCallbackEXT m_vkCreateDebugReportCallbackEXT;
	PFN_vkDestroyDebugReportCallbackEXT m_vkDestroyDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT m_vkDebugReportMessageEXT;
};

#endif