#include <VKL/VKLInstance.h>


VKLInstance::VKLInstance() {
	m_instance = VK_NULL_HANDLE;
	m_allocator = NULL;
	m_debugCallback = NULL;
	m_debug = VK_FALSE;
}

VKLInstance::VKLInstance(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options) {
	m_instance = VK_NULL_HANDLE;
	m_allocator = NULL;
	m_debugCallback = NULL;
	m_debug = VK_FALSE;
	create(vkFunct, options);
}

void VKLInstance::create(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options) {
	if (options != NULL) {
		m_debug = options->m_debug;
		m_allocator = options->m_allocator;
	}

	m_vkGetInstanceProcAddr = vkFunct;

	m_vkCreateInstance = (PFN_vkCreateInstance)m_vkGetInstanceProcAddr(NULL, "vkCreateInstance");
	m_vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)m_vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
	m_vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)m_vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");

	uint32_t layerCount = 0;
	VkLayerProperties* layersAvailable = NULL;
	uint32_t extensionCountEXT = 0;
	VkExtensionProperties* extensionsAvailable = NULL;
	
	if (options != NULL) {
		for (int i = 0; i < options->m_extensions.size(); i++) {
			m_extensions.push_back(options->m_extensions[i]);
		}
	}

	if (m_debug) {
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);

		if (layerCount > 0) {
			layersAvailable = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, layersAvailable);

			for (int i = 0; i < layerCount; ++i) {
				if (strcmp(layersAvailable[i].layerName, "VK_LAYER_LUNARG_standard_validation") == 0) {
					m_layers.push_back("VK_LAYER_LUNARG_standard_validation");
				}
			}

			if (m_layers.size() == 0) {
				printf("Warning: Could not find validation layer! (VK_LAYER_LUNARG_standard_validation)\n");
			}
		} else {
			printf("WARNING: Could not find any layers\n");
		}

		vkEnumerateInstanceExtensionProperties(NULL, &extensionCountEXT, NULL);
		if (extensionCountEXT > 0) {
			extensionsAvailable = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCountEXT);
			vkEnumerateInstanceExtensionProperties(NULL, &extensionCountEXT, extensionsAvailable);

			uint32_t initialExtensionCount = m_extensions.size();

			for (uint32_t i = 0; i < extensionCountEXT; ++i) {
				if (strcmp(extensionsAvailable[i].extensionName, "VK_EXT_debug_report") == 0) {
					m_extensions.push_back("VK_EXT_debug_report");
				}
			}

			if (m_extensions.size() == initialExtensionCount) {
				printf("Warning: Could not get debug extension! (VK_EXT_debug_report)\n");
			}
		} else {
			printf("WARNING: Could not find any extension\n");
		}
		
		printf("Layers:\n");
		for (uint32_t i = 0; i < layerCount; ++i) {
			printf("\tLayer ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, layersAvailable[i].layerName);
			for (uint32_t j = 0; j < m_layers.size(); ++j) {
				if (strcmp(layersAvailable[i].layerName, m_layers[j]) == 0) {
					for (int k = 0; k < 50 - strlen(layersAvailable[i].layerName); k++) {
						printf(" ");
					}
					printf(" - Selected");
				}
			}
			printf("\n");
		}

		printf("Extensions:\n");
		for (uint32_t i = 0; i < extensionCountEXT; ++i) {
			printf("\tExtension ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, extensionsAvailable[i].extensionName);
			for (uint32_t j = 0; j < m_extensions.size(); ++j) {
				if (strcmp(extensionsAvailable[i].extensionName, m_extensions[j]) == 0) {
					for (int k = 0; k < 50 - strlen(extensionsAvailable[i].extensionName); k++) {
						printf(" ");
					}
					printf(" - Selected");
				}
			}
			printf("\n");
		}
	}

	VkApplicationInfo applicationInfo;
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = NULL;
	applicationInfo.pApplicationName = "VKL";
	applicationInfo.applicationVersion = 1;
	applicationInfo.pEngineName = "VKL Engine";
	applicationInfo.engineVersion = 1;
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.enabledLayerCount = m_layers.size();
	instanceInfo.ppEnabledLayerNames = m_layers.data();
	instanceInfo.enabledExtensionCount = m_extensions.size();
	instanceInfo.ppEnabledExtensionNames = m_extensions.data();

	vkCreateInstance(&instanceInfo, m_allocator, &m_instance);

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
	
	__vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)m_vkGetInstanceProcAddr(m_instance, "vkDestroySurfaceKHR");
	__vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	__vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	__vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	__vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)m_vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
}

VkAllocationCallbacks* VKLInstance::allocator() {
	return m_allocator;
}

VkInstance VKLInstance::handle() {
	return m_instance;
}

void VKLInstance::destroy() {
	vkDestroyInstance(m_allocator);
}

VKLInstanceOptions::VKLInstanceOptions() {
	m_allocator = NULL;
	m_debug = VK_FALSE;
}

void VKLInstanceOptions::setAllocator(VkAllocationCallbacks* allocator) {
	m_allocator = allocator;
}

void VKLInstanceOptions::addExtension(char* extension) {
	m_extensions.push_back(extension);
}

void VKLInstanceOptions::addExtensions(char** extensions, uint32_t extensionCount) {
	for (int i = 0; i < extensionCount; i++) {
		m_extensions.push_back(extensions[i]);
	}
}

void VKLInstanceOptions::addExtensions(std::vector<char*> extensions) {
	for (int i = 0; i < extensions.size(); i++) {
		m_extensions.push_back(extensions[i]);
	}
}

void VKLInstanceOptions::setDebug(VkBool32 debug) {
	m_debug = debug;
}

void VKLInstance::vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
	VK_CALL(m_vkCreateInstance(pCreateInfo, pAllocator, pInstance));
}
void VKLInstance::vkDestroyInstance(const VkAllocationCallbacks* pAllocator) {
	m_vkDestroyInstance(m_instance, pAllocator);
}
void VKLInstance::vkEnumeratePhysicalDevices(uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
	VK_CALL(m_vkEnumeratePhysicalDevices(m_instance, pPhysicalDeviceCount, pPhysicalDevices));
}
void VKLInstance::vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {
	m_vkGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}
void VKLInstance::vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties) {
	m_vkGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
}
void VKLInstance::vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) {
	VK_CALL(m_vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties));
}
void VKLInstance::vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
	m_vkGetPhysicalDeviceProperties(physicalDevice, pProperties);
}
void VKLInstance::vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) {
	m_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
void VKLInstance::vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
	m_vkGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}
PFN_vkVoidFunction VKLInstance::vkGetInstanceProcAddr(const char* pName) {
	return m_vkGetInstanceProcAddr(m_instance, pName);
}
void VKLInstance::vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
	VK_CALL(m_vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice));
}
void VKLInstance::vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
	m_vkDestroyDevice(device, pAllocator);
}
void VKLInstance::vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
	VK_CALL(m_vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties));
}
void VKLInstance::vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
	VK_CALL(m_vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties));
}
void VKLInstance::vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
	VK_CALL(m_vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties));
}
void VKLInstance::vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
	VK_CALL(m_vkEnumerateDeviceLayerProperties(physicalDevice, pPropertyCount, pProperties));
}

void VKLInstance::vkCreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	VK_CALL(m_vkCreateDebugReportCallbackEXT(m_instance, pCreateInfo, pAllocator, pCallback));
}
void VKLInstance::vkDestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	m_vkDestroyDebugReportCallbackEXT(m_instance, callback, pAllocator);
}
void VKLInstance::vkDebugReportMessageEXT(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage) {
	m_vkDebugReportMessageEXT(m_instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}