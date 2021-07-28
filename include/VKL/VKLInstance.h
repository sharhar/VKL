#ifndef VKLInstance_h
#define VKLInstance_h

#include "VKL_base.h"

#include <VKL/VKLPhysicalDevice.h>

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

class VKLInstanceCreateInfo : public VKLCreateInfo{
public:
	VKLInstanceCreateInfo();
	
	VKLInstanceCreateInfo& setAllocationCallbacks(VkAllocationCallbacks* allocationCallbacks);
	VKLInstanceCreateInfo& addLayer(const char* layer);
	VKLInstanceCreateInfo& addExtension(const char* extension);
	VKLInstanceCreateInfo& addExtensions(const char** extensions, uint32_t extensionCount);
	VKLInstanceCreateInfo& setDebug(VkBool32 debug);
	VKLInstanceCreateInfo& setProcAddr(PFN_vkGetInstanceProcAddr vkFunc);
	
	bool supportsExtension(const char* extension) const;
	bool supportsLayer(const char* layer) const;
	
	std::vector<VkExtensionProperties> supportedExtensions;
	std::vector<VkLayerProperties> supportedLayers;
	
	std::vector<const char*> layers;
	std::vector<const char*> extensions;
	
	bool isValid() const;
	
	PFN_vkGetInstanceProcAddr procAddr;
	VkBool32 debug;
	VkAllocationCallbacks* allocationCallbacks;
	
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo ci;
private:
	PFN_vkEnumerateInstanceExtensionProperties m_vkEnumerateInstanceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties m_vkEnumerateInstanceLayerProperties;
};

class VKLInstance : public VKLHandle<VkInstance>, public VKLBuilder<VKLInstanceCreateInfo> {
public:
	VKLInstance();
	VKLInstance(const VKLInstanceCreateInfo& createInfo);
	
	const VkAllocationCallbacks* allocationCallbacks() const;
	PFN_vkVoidFunction procAddr(const char* name) const;
	const std::vector<VKLPhysicalDevice>& getPhysicalDevices() const;
	const std::vector<const char*>& getLayers() const;
	const std::vector<const char*>& getExtensions() const;
	void destroySurface(VkSurfaceKHR surface) const;
	
	void destroy();

	VKLInstancePFNS vk;
private:
	const VkAllocationCallbacks* m_allocationCallbacks;

	VkDebugReportCallbackEXT m_debugCallback;

	std::vector<VKLPhysicalDevice> m_physicalDevices;
	
	std::vector<const char*> m_layers;
	std::vector<const char*> m_extensions;

	void _build(const VKLInstanceCreateInfo& createInfo);
};

#endif
