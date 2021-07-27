#include <VKL/VKLInstance.h>
#include <VKL/VKLPhysicalDevice.h>

VKLInstance::VKLInstance() : VKLBuilder<VKLInstanceCreateInfo>("VKLInstance") {
	m_handle = VK_NULL_HANDLE;
	m_allocationCallbacks = NULL;
	m_debugCallback = NULL;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
}

void VKLInstance::_build() {
	vk.GetInstanceProcAddr = ci.procAddr;
	
	vk.CreateInstance = (PFN_vkCreateInstance)procAddr("vkCreateInstance");
	vk.EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)procAddr("vkEnumerateInstanceLayerProperties");
	vk.EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)procAddr("vkEnumerateInstanceExtensionProperties");

	if (ci.debug) {
		ci.addLayer("VK_LAYER_KHRONOS_validation");
		ci.addExtension("VK_EXT_debug_report");
		
		printf("Layers:\n");
		for (uint32_t i = 0; i < ci.supportedLayers.size(); ++i) {
			printf("\tLayer ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, ci.supportedLayers[i].layerName);
			for (uint32_t j = 0; j < ci.layers.size(); ++j) {
				if (strcmp(ci.supportedLayers[i].layerName, ci.layers[j]) == 0) {
					for (int k = 0; k < 50 - strlen(ci.supportedLayers[i].layerName); k++) {
						printf(" ");
					}
					printf(" - Selected");
				}
			}
			printf("\n");
		}

		printf("Extensions:\n");
		for (uint32_t i = 0; i < ci.supportedExtensions.size(); ++i) {
			printf("\tExtension ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, ci.supportedExtensions[i].extensionName);
			for (uint32_t j = 0; j < ci.extensions.size(); ++j) {
				if (strcmp(ci.supportedExtensions[i].extensionName, ci.extensions[j]) == 0) {
					for (int k = 0; k < 50 - strlen(ci.supportedExtensions[i].extensionName); k++) {
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

	VK_CALL(vk.CreateInstance(&ci.ci, m_allocationCallbacks, &m_handle));

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
		m_physicalDevices.push_back(new VKLPhysicalDevice(physicalDevices[i], this));
	}

	free(physicalDevices);
}

VkAllocationCallbacks* VKLInstance::allocationCallbacks() {
	return m_allocationCallbacks;
}

bool VKLInstance::buildable() {
	return ci.procAddr != NULL;
}

PFN_vkVoidFunction VKLInstance::procAddr(const char* name) {
	return vk.GetInstanceProcAddr(m_handle, name);
}

const std::vector<VKLPhysicalDevice*>& VKLInstance::getPhysicalDevices() {
	return m_physicalDevices;
}

void VKLInstance::destroySurface(VkSurfaceKHR surface) {
	vk.DestroySurfaceKHR(m_handle, surface, m_allocationCallbacks);
}

void VKLInstance::destroy() {
	for (int i = 0; i < m_physicalDevices.size(); i++) {
		delete m_physicalDevices[i];
	}

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

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addLayer(char* layer) {
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

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addExtension(char* extension) {
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

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addExtensions(char** extensions, uint32_t extensionCount) {
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

bool VKLInstanceCreateInfo::supportsExtension(char* extension) {
	for(int i = 0; i < supportedExtensions.size(); i++) {
		if (strcmp(supportedExtensions[i].extensionName, extension) == 0) {
			return true;
		}
	}
	
	return false;
}

bool VKLInstanceCreateInfo::supportsLayer(char* layer) {
	for(int i = 0; i < supportedLayers.size(); i++) {
		if (strcmp(supportedLayers[i].layerName, layer) == 0) {
			return true;
		}
	}
	
	return false;
}
