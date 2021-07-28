#include <VKL/VKL.h>

VKLInstance::VKLInstance() : VKLBuilder<VKLInstanceCreateInfo>("VKLInstance") {
	m_handle = VK_NULL_HANDLE;
	m_allocationCallbacks = NULL;
	m_debugCallback = NULL;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
}


VKLInstance::VKLInstance(const VKLInstanceCreateInfo& createInfo) : VKLBuilder<VKLInstanceCreateInfo>("VKLInstance") {
	m_handle = VK_NULL_HANDLE;
	m_allocationCallbacks = NULL;
	m_debugCallback = NULL;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
	this->build(createInfo);
}

void VKLInstance::_build(const VKLInstanceCreateInfo& createInfo) {
	vk.GetInstanceProcAddr = createInfo.procAddr;
	
	vk.CreateInstance = (PFN_vkCreateInstance)procAddr("vkCreateInstance");
	vk.EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)procAddr("vkEnumerateInstanceLayerProperties");
	vk.EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)procAddr("vkEnumerateInstanceExtensionProperties");

	m_extensions.insert(m_extensions.end(), createInfo.extensions.begin(), createInfo.extensions.end());
	m_layers.insert(m_layers.end(), createInfo.layers.begin(), createInfo.layers.end());
	
	if (createInfo.debug) {
		if(createInfo.supportsLayer("VK_LAYER_KHRONOS_validation")) {
			m_layers.push_back("VK_LAYER_KHRONOS_validation");
		}
		if(createInfo.supportsExtension("VK_EXT_debug_report")) {
			m_extensions.push_back("VK_EXT_debug_report");
		}
		
		printf("Layers:\n");
		for (uint32_t i = 0; i < createInfo.supportedLayers.size(); ++i) {
			printf("\tLayer ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, createInfo.supportedLayers[i].layerName);
			for (uint32_t j = 0; j < createInfo.layers.size(); ++j) {
				if (strcmp(createInfo.supportedLayers[i].layerName, createInfo.layers[j]) == 0) {
					for (int k = 0; k < 50 - strlen(createInfo.supportedLayers[i].layerName); k++) {
						printf(" ");
					}
					printf(" - Selected");
				}
			}
			printf("\n");
		}

		printf("Extensions:\n");
		for (uint32_t i = 0; i < createInfo.supportedExtensions.size(); ++i) {
			printf("\tExtension ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, createInfo.supportedExtensions[i].extensionName);
			for (uint32_t j = 0; j < createInfo.extensions.size(); ++j) {
				if (strcmp(createInfo.supportedExtensions[i].extensionName, createInfo.extensions[j]) == 0) {
					for (int k = 0; k < 50 - strlen(createInfo.supportedExtensions[i].extensionName); k++) {
						printf(" ");
					}
					printf(" - Selected");
				}
			}
			printf("\n");
		}
		/*
		printf("\n");
		for(int i = 0; i < instance.getPhysicalDevices().size(); i++) {
			VKLPhysicalDevice* physicalDevice = instance.getPhysicalDevices()[i];
			printf("Device %d (%s):\n", i, physicalDevice->getProperties().deviceName);
			
			printf("\tExtensions:\n");
			for (int j = 0; j < physicalDevice->getExtensions().size(); j++) {
				VkExtensionProperties extensionProps = physicalDevice->getExtensions()[j];
				
				printf("\t\t%d: %s\n", j, extensionProps.extensionName);
			}
			printf("\n");
		}
		*/
	}

	VK_CALL(vk.CreateInstance(&createInfo.ci, m_allocationCallbacks, &m_handle));

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

	VK_CALL(vk.EnumeratePhysicalDevices(m_handle, &physicalDeviceCount, NULL));
	physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice*) * physicalDeviceCount);
	VK_CALL(vk.EnumeratePhysicalDevices(m_handle, &physicalDeviceCount, physicalDevices));

	for (int i = 0; i < physicalDeviceCount; i++) {
		m_physicalDevices.push_back(VKLPhysicalDevice(physicalDevices[i], this));
	}

	free(physicalDevices);
}

const VkAllocationCallbacks* VKLInstance::allocationCallbacks() const {
	return m_allocationCallbacks;
}

PFN_vkVoidFunction VKLInstance::procAddr(const char* name) const {
	return vk.GetInstanceProcAddr(m_handle, name);
}

const std::vector<VKLPhysicalDevice>& VKLInstance::getPhysicalDevices() const {
	return m_physicalDevices;
}
const std::vector<const char*>& VKLInstance::getLayers() const {
	return m_layers;
}

const std::vector<const char*>& VKLInstance::getExtensions() const {
	return m_extensions;
}

void VKLInstance::destroySurface(VkSurfaceKHR surface) const {
	vk.DestroySurfaceKHR(m_handle, surface, m_allocationCallbacks);
}

void VKLInstance::destroy() {
	vk.DestroyInstance(m_handle, m_allocationCallbacks);
}

VKLInstanceCreateInfo::VKLInstanceCreateInfo() {
	memset(&appInfo, 0, sizeof(VkApplicationInfo));
	memset(&ci, 0, sizeof(VkInstanceCreateInfo));
	
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = NULL;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = NULL;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	
	ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	ci.pNext = NULL;
	ci.flags = 0;
	ci.pApplicationInfo = &appInfo;
	
	procAddr = NULL;
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::setProcAddr(PFN_vkGetInstanceProcAddr vkFunc) {
	procAddr = vkFunc;
	
	m_vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)procAddr(NULL, "vkEnumerateInstanceLayerProperties");
	m_vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)procAddr(NULL, "vkEnumerateInstanceExtensionProperties");
	
	uint32_t layerCount = 0;
	m_vkEnumerateInstanceLayerProperties(&layerCount, NULL);
	supportedLayers.resize(layerCount);
	m_vkEnumerateInstanceLayerProperties(&layerCount, supportedLayers.data());
	
	uint32_t extensionCount = 0;
	m_vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	supportedExtensions.resize(extensionCount);
	m_vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, supportedExtensions.data());
	
	//if(supportsExtension("VK_KHR_get_physical_device_properties2")) {
	//	addExtension("VK_KHR_get_physical_device_properties2");
	//}
	
	return *this;
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::setAllocationCallbacks(VkAllocationCallbacks* allocationCallbacks) {
	this->allocationCallbacks = allocationCallbacks;
	return *this;
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addLayer(const char* layer) {
	if(procAddr == NULL) {
		printf("Must set procAddr before adding layers!\n");
		return *this;
	}
	
	if(supportsLayer(layer)) {
		layers.push_back(layer);
		
		ci.enabledLayerCount= layers.size();
		ci.ppEnabledLayerNames = layers.data();
	} else {
		printf("Warning: %s layer is not supported by instance\n", layer);
	}
	
	return *this;
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addExtension(const char* extension) {
	if(procAddr == NULL) {
		printf("Must set procAddr before adding extensions!\n");
		return *this;
	}
	
	if(supportsExtension(extension)) {
		extensions.push_back(extension);
		
		ci.enabledExtensionCount = extensions.size();
		ci.ppEnabledExtensionNames = extensions.data();
	} else {
		printf("Warning: %s extension is not supported by instance\n", extension);
	}
	
	return *this;
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addExtensions(const char** extensions, uint32_t extensionCount) {
	if(procAddr == NULL) {
		printf("Must set procAddr before adding extensions!\n");
		return *this;
	}
	
	for(int  i = 0; i < extensionCount; i++) {
		if(supportsExtension(extensions[i])) {
			this->extensions.push_back(extensions[i]);
		} else {
			printf("Warning: %s extension is not supported by instance\n", extensions[i]);
		}
	}
	
	ci.enabledExtensionCount = this->extensions.size();
	ci.ppEnabledExtensionNames = this->extensions.data();
	
	return *this;
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::setDebug(VkBool32 debug) {
	this->debug = debug;
	return *this;
}

bool VKLInstanceCreateInfo::supportsExtension(const char* extension) const {
	for(int i = 0; i < supportedExtensions.size(); i++) {
		if (strcmp(supportedExtensions[i].extensionName, extension) == 0) {
			return true;
		}
	}
	
	return false;
}

bool VKLInstanceCreateInfo::supportsLayer(const char* layer) const {
	for(int i = 0; i < supportedLayers.size(); i++) {
		if (strcmp(supportedLayers[i].layerName, layer) == 0) {
			return true;
		}
	}
	
	return false;
}

bool VKLInstanceCreateInfo::isValid() const {
	return procAddr != NULL;
}
