#include <VKL/VKL.h>

VKLInstance::VKLInstance() : VKLCreator<VKLInstanceCreateInfo>("VKLInstance") {
	m_handle = VK_NULL_HANDLE;
	m_allocationCallbacks = NULL;
	m_debugCallback = NULL;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
}

VKLInstance::VKLInstance(const VKLInstanceCreateInfo& createInfo) : VKLCreator<VKLInstanceCreateInfo>("VKLInstance") {
	m_handle = VK_NULL_HANDLE;
	m_allocationCallbacks = NULL;
	m_debugCallback = NULL;
	memset(&vk, 0, sizeof(VKLInstancePFNS));
	this->create(createInfo);
}

void VKLInstance::_create(const VKLInstanceCreateInfo& createInfo) {
	vk.GetInstanceProcAddr = createInfo.m_procAddr;
	
	vk.CreateInstance = (PFN_vkCreateInstance)procAddr("vkCreateInstance");
	vk.EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)procAddr("vkEnumerateInstanceLayerProperties");
	vk.EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)procAddr("vkEnumerateInstanceExtensionProperties");

	m_extensions.insert(m_extensions.end(), createInfo.m_extensions.begin(), createInfo.m_extensions.end());
	m_layers.insert(m_layers.end(), createInfo.m_layers.begin(), createInfo.m_layers.end());

	VK_CALL(vk.CreateInstance(&createInfo.m_createInfo, m_allocationCallbacks, &m_handle));

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

#ifdef VKL_SURFACE_WIN32
	vk.CreateWin32SurfaceKHR = procAddr("vkCreateWin32SurfaceKHR");
	vk.GetPhysicalDeviceWin32PresentationSupportKHR = procAddr("vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif

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

void VKLInstance::_destroy() {
	vk.DestroyInstance(m_handle, m_allocationCallbacks);
}

VKLInstanceCreateInfo::VKLInstanceCreateInfo() {
	memset(&m_appInfo, 0, sizeof(VkApplicationInfo));
	memset(&m_createInfo, 0, sizeof(VkInstanceCreateInfo));
	
	m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	m_appInfo.pNext = NULL;
	m_appInfo.pApplicationName = NULL;
	m_appInfo.applicationVersion = 1;
	m_appInfo.pEngineName = NULL;
	m_appInfo.engineVersion = 1;
	m_appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	
	m_createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	m_createInfo.pNext = NULL;
	m_createInfo.flags = 0;
	m_createInfo.pApplicationInfo = &m_appInfo;
	
	m_procAddr = NULL;
	m_debug = VK_FALSE;
}

void VKLInstanceCreateInfo::printSelections() {
	if (!validate()) {
		printf("VKLInstanceCreateInfo not valid\n");
		return;
	}

	_printSelections();
}

void VKLInstanceCreateInfo::_printSelections() {
	printf("Layers:\n");
	for (uint32_t i = 0; i < supportedLayers.size(); ++i) {
		printf("\tLayer ");
		if (i < 10) {
			printf(" ");
		}
		printf(" %d: %s", i, supportedLayers[i].layerName);
		for (uint32_t j = 0; j < m_layers.size(); ++j) {
			if (strcmp(supportedLayers[i].layerName, m_layers[j]) == 0) {
				for (int k = 0; k < 50 - strlen(supportedLayers[i].layerName); k++) {
					printf(" ");
				}
				printf(" - Selected");
			}
		}
		printf("\n");
	}

	printf("Extensions:\n");
	for (uint32_t i = 0; i < supportedExtensions.size(); ++i) {
		printf("\tExtension ");
		if (i < 10) {
			printf(" ");
		}
		printf(" %d: %s", i, supportedExtensions[i].extensionName);
		for (uint32_t j = 0; j < m_extensions.size(); ++j) {
			if (strcmp(supportedExtensions[i].extensionName, m_extensions[j]) == 0) {
				for (int k = 0; k < 50 - strlen(supportedExtensions[i].extensionName); k++) {
					printf(" ");
				}
				printf(" - Selected");
			}
		}
		printf("\n");
	}
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::procAddr(PFN_vkGetInstanceProcAddr vkFunc) {
	m_procAddr = vkFunc;
	return invalidate();
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::allocationCallbacks(VkAllocationCallbacks* allocationCallbacks) {
	m_allocationCallbacks = allocationCallbacks;
	return invalidate();
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addLayer(const char* layer) {
	for (const char* lay : m_layers) {
		if (strcmp(lay, layer) == 0) {
			return invalidate();
		}
	}

	m_layers.push_back(layer);
	return invalidate();
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addExtension(const char* extension) {
	for (const char* ext : m_extensions) {
		if (strcmp(ext, extension) == 0) {
			return invalidate();
		}
	}
	m_extensions.push_back(extension);
	return invalidate();
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::addExtensions(const char** extensions, uint32_t extensionCount) {
	for(int  i = 0; i < extensionCount; i++) {
		m_extensions.push_back(extensions[i]);
	}
	
	return invalidate();
}

VKLInstanceCreateInfo& VKLInstanceCreateInfo::debug(VkBool32 debug) {
	m_debug = debug;
	return invalidate();
}

bool VKLInstanceCreateInfo::supportsExtension(const char* extension) {
	if (!validate()) {
		printf("VKLInstanceCreateInfo not valid\n");
		return false;
	}
	
	return _supportsExtension(extension);
}

bool VKLInstanceCreateInfo::supportsLayer(const char* layer)  {
	if (!validate()) {
		printf("VKLInstanceCreateInfo not valid\n");
		return false;
	}
	return _supportsLayer(layer);
}

bool VKLInstanceCreateInfo::_supportsExtension(const char* extension) {
	for (int i = 0; i < supportedExtensions.size(); i++) {
		if (strcmp(supportedExtensions[i].extensionName, extension) == 0) {
			return true;
		}
	}

	return false;
}

bool VKLInstanceCreateInfo::_supportsLayer(const char* layer) {
	for (int i = 0; i < supportedLayers.size(); i++) {
		if (strcmp(supportedLayers[i].layerName, layer) == 0) {
			return true;
		}
	}

	return false;
}

bool VKLInstanceCreateInfo::_validate() {
#ifdef VKL_VALIDATION
	if (m_procAddr == NULL) {
		printf("VKL Validation Error: VKLInstanceCreateInfo::procAddr was not set!\n");
		return false;
	}
#endif

	m_vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)m_procAddr(NULL, "vkEnumerateInstanceLayerProperties");
	m_vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)m_procAddr(NULL, "vkEnumerateInstanceExtensionProperties");

	uint32_t layerCount = 0;
	m_vkEnumerateInstanceLayerProperties(&layerCount, NULL);
	supportedLayers.resize(layerCount);
	m_vkEnumerateInstanceLayerProperties(&layerCount, supportedLayers.data());

	uint32_t extensionCount = 0;
	m_vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	supportedExtensions.resize(extensionCount);
	m_vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, supportedExtensions.data());

#ifdef VKL_VALIDATION
	for (const char* lay : m_layers) {
		if (!_supportsLayer(lay)) {
			printf("VKL Validation Error: Layer '%s' is not supported by instance!\n", lay);
			return false;
		}
	}

	for (const char* ext : m_extensions) {
		if (!_supportsExtension(ext)) {
			printf("VKL Validation Error: Extension '%s' is not supported by instance!\n", ext);
			return false;
		}
	}
#endif

	if (_supportsExtension("VK_KHR_get_physical_device_properties2")) {
		addExtension("VK_KHR_get_physical_device_properties2");
	}

	if (m_debug) {
		if (_supportsExtension("VK_EXT_debug_report")) {
			addExtension("VK_EXT_debug_report");
		}

		if (_supportsLayer("VK_LAYER_KHRONOS_validation")) {
			addLayer("VK_LAYER_KHRONOS_validation");
		}

		if (_supportsLayer("VK_LAYER_LUNARG_monitor")) {
			addLayer("VK_LAYER_LUNARG_monitor");
		}

		_printSelections();
	}

	m_createInfo.enabledLayerCount = m_layers.size();
	m_createInfo.ppEnabledLayerNames = m_layers.data();

	m_createInfo.enabledExtensionCount = m_extensions.size();
	m_createInfo.ppEnabledExtensionNames = m_extensions.data();

	return true;
}
