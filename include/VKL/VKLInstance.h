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

class VKLInstance : public VKLObject<VkInstance> {
public:
	VKLInstance();
	
	VKLInstance& ciSetAllocationCallbacks(VkAllocationCallbacks* allocationCallbacks);
	VKLInstance& ciAddExtension(char* extension);
	VKLInstance& ciAddExtensions(char** extensions, uint32_t extensionCount);
	VKLInstance& ciAddExtensions(std::vector<char*> extensions);
	VKLInstance& ciSetDebug(VkBool32 debug);
	VKLInstance& ciSetProcAddr(PFN_vkGetInstanceProcAddr vkFunc);
	
	VkAllocationCallbacks* allocationCallbacks();
	PFN_vkVoidFunction procAddr(const char* name);
	const std::vector<char*>& getLayers();
	const std::vector<char*>& getExtensions();
	const std::vector<VKLPhysicalDevice*>& getPhysicalDevices();
	void destroy();

	VKLInstancePFNS vk;
private:
	VkAllocationCallbacks* m_allocationCallbacks;

	VkDebugReportCallbackEXT m_debugCallback;

	std::vector<char*> m_layers;
	std::vector<char*> m_extensions;
	std::vector<VKLPhysicalDevice*> m_physicalDevices;

	VkBool32 m_debug;
	
	void _build();
	
	std::vector<char*> m_configExtensions;
};

#endif
