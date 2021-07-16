#ifndef VKLInstance_h
#define VKLInstance_h

#include "VKL_base.h"

#include <vector>
#include <VKL/VKLPhysicalDevice.h>

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

class VKLInstance {
private:
	VkInstance m_instance;

	VkAllocationCallbacks* m_allocator;

	VkDebugReportCallbackEXT m_debugCallback;

	std::vector<char*> m_layers;
	std::vector<char*> m_extensions;
	std::vector<VKLPhysicalDevice> m_physicalDevices;

	VkBool32 m_debug;
public:
	VKLInstance();
	VKLInstance(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options);

	void create(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options);
	VkAllocationCallbacks* allocator();
	PFN_vkVoidFunction procAddr(const char* name);
	const std::vector<char*>& getLayers();
	const std::vector<char*>& getExtensions();
	const std::vector<VKLPhysicalDevice>& getPhysicalDevices();
	VkInstance handle();
	void destroy();

	VKLInstancePFNS vk;
};

#endif