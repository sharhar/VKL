#ifndef VKLInstance_h
#define VKLInstance_h

#include "VKL_base.h"

#include <vector>

typedef struct VKLInstancePFNS {
	PFN_vkCreateInstance CreateInstance;
	PFN_vkDestroyInstance DestroyInstance;
	PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices;
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
	PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties;

	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallbackEXT;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT DebugReportMessageEXT;

	PFN_vkGetPhysicalDeviceFeatures GetPhysicalDeviceFeatures;
	PFN_vkGetPhysicalDeviceFormatProperties GetPhysicalDeviceFormatProperties;
	PFN_vkGetPhysicalDeviceImageFormatProperties GetPhysicalDeviceImageFormatProperties;
	PFN_vkGetPhysicalDeviceProperties GetPhysicalDeviceProperties;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties GetPhysicalDeviceSparseImageFormatProperties;

	PFN_vkCreateDevice CreateDevice;
	PFN_vkDestroyDevice DestroyDevice;
	PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties;
	PFN_vkEnumerateDeviceLayerProperties EnumerateDeviceLayerProperties;

	PFN_vkDestroySurfaceKHR DestroySurfaceKHR;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR GetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR GetPhysicalDeviceSurfacePresentModesKHR;
} __VKLInstancePFNS;

typedef struct VKLInstanceCreateInfo {
	VKLInstanceCreateInfo();
	
	VKLInstanceCreateInfo& setAllocationCallbacks(VkAllocationCallbacks* allocationCallbacks);
	VKLInstanceCreateInfo& addLayer(char* layer);
	VKLInstanceCreateInfo& addExtension(char* extension);
	VKLInstanceCreateInfo& addExtensions(char** extensions, uint32_t extensionCount);
	VKLInstanceCreateInfo& setDebug(VkBool32 debug);
	VKLInstanceCreateInfo& setProcAddr(PFN_vkGetInstanceProcAddr vkFunc);
	
	std::vector<char*> layers;
	std::vector<char*> extensions;
	
	std::vector<VkExtensionProperties> supportedExtensions;
	std::vector<VkLayerProperties> supportedLayers;
	
	PFN_vkGetInstanceProcAddr procAddr;
	VkBool32 debug;
	VkAllocationCallbacks* allocationCallbacks;
	
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo ci;
private:
	PFN_vkEnumerateInstanceExtensionProperties m_vkEnumerateInstanceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties m_vkEnumerateInstanceLayerProperties;
	
	bool supportsExtension(char* extension);
	bool supportsLayer(char* layer);
} VKLInstanceCreateInfo;

class VKLInstance : public VKLHandle<VkInstance>, public VKLBuilder<VKLInstanceCreateInfo> {
public:
	VKLInstance();
	
	VkAllocationCallbacks* allocationCallbacks();
	PFN_vkVoidFunction procAddr(const char* name);
	const std::vector<VKLPhysicalDevice*>& getPhysicalDevices();
	
	bool buildable();
	
	void destroySurface(VkSurfaceKHR surface);
	
	void destroy();

	VKLInstancePFNS vk;
private:
	VkAllocationCallbacks* m_allocationCallbacks;

	VkDebugReportCallbackEXT m_debugCallback;

	std::vector<VKLPhysicalDevice*> m_physicalDevices;

	void _build();
};

#endif
