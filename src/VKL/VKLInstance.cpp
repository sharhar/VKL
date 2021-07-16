#include <VKL/VKLInstance.h>
#include <VKL/VKLPhysicalDevice.h>

VKLInstance::VKLInstance() {
	m_instance = VK_NULL_HANDLE;
	m_allocator = NULL;
	m_debugCallback = NULL;
	m_debug = VK_FALSE;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
}

VKLInstance::VKLInstance(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options) {
	m_instance = VK_NULL_HANDLE;
	m_allocator = NULL;
	m_debugCallback = NULL;
	m_debug = VK_FALSE;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
	create(vkFunct, options);
}

void VKLInstance::create(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options) {
	if (options != NULL) {
		m_debug = options->m_debug;
		m_allocator = options->m_allocator;
	}

	vk.GetInstanceProcAddr = vkFunct;

	vk.CreateInstance = (PFN_vkCreateInstance)procAddr("vkCreateInstance");
	vk.EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)procAddr("vkEnumerateInstanceLayerProperties");
	vk.EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)procAddr("vkEnumerateInstanceExtensionProperties");

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
		VK_CALL(vk.EnumerateInstanceLayerProperties(&layerCount, NULL));

		if (layerCount > 0) {
			layersAvailable = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
			VK_CALL(vk.EnumerateInstanceLayerProperties(&layerCount, layersAvailable));

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

		VK_CALL(vk.EnumerateInstanceExtensionProperties(NULL, &extensionCountEXT, NULL));
		if (extensionCountEXT > 0) {
			extensionsAvailable = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCountEXT);
			VK_CALL(vk.EnumerateInstanceExtensionProperties(NULL, &extensionCountEXT, extensionsAvailable));

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

	VK_CALL(vk.CreateInstance(&instanceInfo, m_allocator, &m_instance));

	vk.DestroyInstance = (PFN_vkDestroyInstance)procAddr("vkDestroyInstance");
	vk.EnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)procAddr("vkEnumeratePhysicalDevices");

	vk.CreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)procAddr("vkCreateDebugReportCallbackEXT");
	vk.DestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)procAddr("vkDestroyDebugReportCallbackEXT");
	vk.DebugReportMessageEXT = (PFN_vkDebugReportMessageEXT)procAddr("vkDebugReportMessageEXT");

	vk.GetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)procAddr("vkGetPhysicalDeviceFeatures");
	vk.GetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)procAddr("vkGetPhysicalDeviceFormatProperties");
	vk.GetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)procAddr("vkGetPhysicalDeviceImageFormatProperties");
	vk.GetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)procAddr("vkGetPhysicalDeviceProperties");
	vk.GetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)procAddr("vkGetPhysicalDeviceQueueFamilyProperties");
	vk.GetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)procAddr("vkGetPhysicalDeviceMemoryProperties");
	vk.GetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)procAddr("vkGetPhysicalDeviceSparseImageFormatProperties");

	vk.CreateDevice = (PFN_vkCreateDevice)procAddr("vkCreateDevice");
	vk.DestroyDevice = (PFN_vkDestroyDevice)procAddr("vkDestroyDevice");
	vk.EnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)procAddr("vkEnumerateDeviceExtensionProperties");
	vk.EnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)procAddr("vkEnumerateDeviceLayerProperties");
	
	vk.DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)procAddr("vkDestroySurfaceKHR");
	vk.GetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)procAddr("vkGetPhysicalDeviceSurfaceSupportKHR");
	vk.GetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)procAddr("vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	vk.GetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)procAddr("vkGetPhysicalDeviceSurfaceFormatsKHR");
	vk.GetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)procAddr("vkGetPhysicalDeviceSurfacePresentModesKHR");

	VkPhysicalDevice* physicalDevices = NULL;
	uint32_t physicalDeviceCount = 0;

	VK_CALL(vk.EnumeratePhysicalDevices(m_instance, &physicalDeviceCount, NULL));
	physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice*) * physicalDeviceCount);
	VK_CALL(vk.EnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices));

	for (int i = 0; i < physicalDeviceCount; i++) {
		m_physicalDevices.push_back(VKLPhysicalDevice(physicalDevices[i], *this));
	}

	free(physicalDevices);
}

VkAllocationCallbacks* VKLInstance::allocator() {
	return m_allocator;
}

PFN_vkVoidFunction VKLInstance::procAddr(const char* name) {
	return vk.GetInstanceProcAddr(m_instance, name);
}

const std::vector<char*>& VKLInstance::getLayers() {
	return m_layers;
}

const std::vector<char*>& VKLInstance::getExtensions() {
	return m_extensions;
}

const std::vector<VKLPhysicalDevice>& VKLInstance::getPhysicalDevices() {
	return m_physicalDevices;
}

VkInstance VKLInstance::handle() {
	return m_instance;
}

void VKLInstance::destroy() {
	vk.DestroyInstance(m_instance, m_allocator);
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
