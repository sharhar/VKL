#ifdef FUNC_DESC
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
void vkDestroySurfaceKHR(VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator);
void vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported);
void vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities);
void vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats);
void vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes);
void vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPropertiesKHR* pProperties);
void vkGetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties);
void vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex, uint32_t* pDisplayCount, VkDisplayKHR* pDisplays);
void vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, uint32_t* pPropertyCount, VkDisplayModePropertiesKHR* pProperties);
void vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, const VkDisplayModeCreateInfoKHR*pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode);
void vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode, uint32_t planeIndex, VkDisplayPlaneCapabilitiesKHR* pCapabilities);
void vkCreateDisplayPlaneSurfaceKHR(const VkDisplaySurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);
#endif

#ifdef FUNC_IMPLS
void VKLInstance::vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance){
	VK_CALL(m_vkCreateInstance(pCreateInfo, pAllocator, pInstance));
}
void VKLInstance::vkDestroyInstance(const VkAllocationCallbacks* pAllocator){
	m_vkDestroyInstance(m_instance, pAllocator);
}
void VKLInstance::vkEnumeratePhysicalDevices(uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices){
	VK_CALL(m_vkEnumeratePhysicalDevices(m_instance, pPhysicalDeviceCount, pPhysicalDevices));
}
void VKLInstance::vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures){
	m_vkGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}
void VKLInstance::vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties){
	m_vkGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
}
void VKLInstance::vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties){
	VK_CALL(m_vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties));
}
void VKLInstance::vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties){
	m_vkGetPhysicalDeviceProperties(physicalDevice, pProperties);
}
void VKLInstance::vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties){
	m_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
void VKLInstance::vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties){
	m_vkGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}
PFN_vkVoidFunction VKLInstance::vkGetInstanceProcAddr(const char* pName){
	return m_vkGetInstanceProcAddr(m_instance, pName);
}
void VKLInstance::vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice){
	VK_CALL(m_vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice));
}
void VKLInstance::vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator){
	m_vkDestroyDevice(device, pAllocator);
}
void VKLInstance::vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties){
	VK_CALL(m_vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties));
}
void VKLInstance::vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties){
	VK_CALL(m_vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties));
}
void VKLInstance::vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties){
	VK_CALL(m_vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties));
}
void VKLInstance::vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties){
	VK_CALL(m_vkEnumerateDeviceLayerProperties(physicalDevice, pPropertyCount, pProperties));
}
void VKLInstance::vkCreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback){
	VK_CALL(m_vkCreateDebugReportCallbackEXT(m_instance, pCreateInfo, pAllocator, pCallback));
}
void VKLInstance::vkDestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator){
	m_vkDestroyDebugReportCallbackEXT(m_instance, callback, pAllocator);
}
void VKLInstance::vkDebugReportMessageEXT(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage){
	m_vkDebugReportMessageEXT(m_instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}
void VKLInstance::vkDestroySurfaceKHR(VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator){
	m_vkDestroySurfaceKHR(m_instance, surface, pAllocator);
}
void VKLInstance::vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported){
	VK_CALL(m_vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported));
}
void VKLInstance::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities){
	VK_CALL(m_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities));
}
void VKLInstance::vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats){
	VK_CALL(m_vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats));
}
void VKLInstance::vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes){
	VK_CALL(m_vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes));
}
void VKLInstance::vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPropertiesKHR* pProperties){
	VK_CALL(m_vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties));
}
void VKLInstance::vkGetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties){
	VK_CALL(m_vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties));
}
void VKLInstance::vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex, uint32_t* pDisplayCount, VkDisplayKHR* pDisplays){
	VK_CALL(m_vkGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays));
}
void VKLInstance::vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, uint32_t* pPropertyCount, VkDisplayModePropertiesKHR* pProperties){
	VK_CALL(m_vkGetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties));
}
void VKLInstance::vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, const VkDisplayModeCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode){
	VK_CALL(m_vkCreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode));
}
void VKLInstance::vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode, uint32_t planeIndex, VkDisplayPlaneCapabilitiesKHR* pCapabilities){
	VK_CALL(m_vkGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities));
}
void VKLInstance::vkCreateDisplayPlaneSurfaceKHR(const VkDisplaySurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface){
	VK_CALL(m_vkCreateDisplayPlaneSurfaceKHR(m_instance, pCreateInfo, pAllocator, pSurface));
}
#endif

#ifdef PFNS
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
PFN_vkDestroySurfaceKHR m_vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR m_vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR m_vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR m_vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR m_vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceDisplayPropertiesKHR m_vkGetPhysicalDeviceDisplayPropertiesKHR;
PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR m_vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
PFN_vkGetDisplayPlaneSupportedDisplaysKHR m_vkGetDisplayPlaneSupportedDisplaysKHR;
PFN_vkGetDisplayModePropertiesKHR m_vkGetDisplayModePropertiesKHR;
PFN_vkCreateDisplayModeKHR m_vkCreateDisplayModeKHR;
PFN_vkGetDisplayPlaneCapabilitiesKHR m_vkGetDisplayPlaneCapabilitiesKHR;
PFN_vkCreateDisplayPlaneSurfaceKHR m_vkCreateDisplayPlaneSurfaceKHR;
#endif

#ifdef PROC_ADDRS
m_vkDestroyInstance = (PFN_vkDestroyInstance)m_vkGetInstanceProcAddr(m_instance, "vkDestroyInstance");
m_vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)m_vkGetInstanceProcAddr(m_instance, "vkEnumeratePhysicalDevices");
m_vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceFeatures");
m_vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceFormatProperties");
m_vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceImageFormatProperties");
m_vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceProperties");
m_vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceQueueFamilyProperties");
m_vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceMemoryProperties");
m_vkCreateDevice = (PFN_vkCreateDevice)m_vkGetInstanceProcAddr(m_instance, "vkCreateDevice");
m_vkDestroyDevice = (PFN_vkDestroyDevice)m_vkGetInstanceProcAddr(m_instance, "vkDestroyDevice");
m_vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)m_vkGetInstanceProcAddr(m_instance, "vkEnumerateDeviceExtensionProperties");
m_vkEnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)m_vkGetInstanceProcAddr(m_instance, "vkEnumerateDeviceLayerProperties");
m_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)m_vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
m_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)m_vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
m_vkDebugReportMessageEXT = (PFN_vkDebugReportMessageEXT)m_vkGetInstanceProcAddr(m_instance, "vkDebugReportMessageEXT");
m_vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)m_vkGetInstanceProcAddr(m_instance, "vkDestroySurfaceKHR");
m_vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
m_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
m_vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
m_vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
m_vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");
m_vkGetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
m_vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
m_vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetDisplayModePropertiesKHR");
m_vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)m_vkGetInstanceProcAddr(m_instance, "vkCreateDisplayModeKHR");
m_vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetDisplayPlaneCapabilitiesKHR");
m_vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)m_vkGetInstanceProcAddr(m_instance, "vkCreateDisplayPlaneSurfaceKHR");
#endif
