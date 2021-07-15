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

#define PROC_ADDRS
#include <VKL/VKLInstance_autogen.h>
#undef PROC_ADDRS
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

#define FUNC_IMPLS
#include <VKL/VKLInstance_autogen.h>
#undef FUNC_IMPLS

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