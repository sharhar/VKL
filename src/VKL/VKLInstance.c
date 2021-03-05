#include <VKL/VKL.h>

VKAPI_ATTR VkBool32 VKAPI_CALL __DebugReportCallback(VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
	int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {

	printf("%s %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}

int vklCreateInstance(VKLInstance** pInstace, VkAllocationCallbacks* allocator, char* wsiExtName, PFN_vkGetInstanceProcAddr vkFunct, VkBool32 debug) {
	*pInstace = (VKLInstance*)malloc_c(sizeof(VKLInstance));
	VKLInstance* instance = *pInstace;

	instance->debug = debug;
	instance->allocator = allocator;
	instance->pvkGetInstanceProcAddr = vkFunct;

	instance->pvkCreateInstance = (PFN_vkCreateInstance)instance->pvkGetInstanceProcAddr(NULL, "vkCreateInstance");
	instance->pvkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)instance->pvkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
	instance->pvkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)instance->pvkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");

	uint32_t layerCount = 0;
	VkLayerProperties* layersAvailable;
	uint32_t extensionCountEXT = 0;
	VkExtensionProperties* extensionsAvailable;

	if (debug) {
		instance->pvkEnumerateInstanceLayerProperties(&layerCount, NULL);
		if (layerCount == 0) {
			printf("Could not find any layers!\n");
			return -1;
		}

		layersAvailable = (VkLayerProperties*)malloc_c(sizeof(VkLayerProperties) * layerCount);
		instance->pvkEnumerateInstanceLayerProperties(&layerCount, layersAvailable);

		uint8_t foundValidation = 0;
		for (int i = 0; i < layerCount; ++i) {
			if (strcmp(layersAvailable[i].layerName, "VK_LAYER_LUNARG_standard_validation") == 0) {
				foundValidation = 1;
			}
		}

		if (!foundValidation) {
			printf("Warning: Could not find validation layers! (VK_LAYER_LUNARG_standard_validation)\n");
		}

		instance->layers = (char**)malloc_c(sizeof(char*) * 1);
		instance->layers[0] = "VK_LAYER_LUNARG_standard_validation";
		instance->layerCount = 1;
	}
	else {
		instance->layers = NULL;
		instance->layerCount = 0;
	}

	uint32_t extensionCount;
	char** extensions;

	extensionCount = 2;
	extensions = malloc_c(sizeof(char*) * 2);
	extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
	extensions[1] = wsiExtName;

	if (debug) {
		instance->pvkEnumerateInstanceExtensionProperties(NULL, &extensionCountEXT, NULL);
		extensionsAvailable = (VkExtensionProperties*)malloc_c(sizeof(VkExtensionProperties) * extensionCountEXT);
		instance->pvkEnumerateInstanceExtensionProperties(NULL, &extensionCountEXT, extensionsAvailable);

		char *extentionEXT = "VK_EXT_debug_report";
		uint32_t foundExtensions = 0;
		for (uint32_t i = 0; i < extensionCountEXT; ++i) {
			if (strcmp(extensionsAvailable[i].extensionName, extentionEXT) == 0) {
				foundExtensions++;
			}
		}

		if (!foundExtensions) {
			printf("Warning: Could not get required extentions! (%s)\n", extentionEXT);
		}


		uint32_t preExtentionCount = extensionCount;
		char** preExtentions = extensions;

		extensionCount = preExtentionCount + 1;
		extensions = (char**)malloc_c(sizeof(char*) * extensionCount);
		for (int i = 0; i < preExtentionCount; i++) {
			extensions[i] = preExtentions[i];
		}
		extensions[preExtentionCount] = extentionEXT;
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
	instanceInfo.enabledLayerCount = instance->layerCount;
	instanceInfo.ppEnabledLayerNames = (const char**)instance->layers;
	instanceInfo.enabledExtensionCount = extensionCount;
	instanceInfo.ppEnabledExtensionNames = (const char**)extensions;

	if (debug) {
		printf("Layers:\n");
		for (uint32_t i = 0; i < layerCount; ++i) {
			printf("\tLayer ");
			if (i < 10) {
				printf(" ");
			}
			printf(" %d: %s", i, layersAvailable[i].layerName);
			for (uint32_t j = 0; j < instance->layerCount; ++j) {
				if (strcmp(layersAvailable[i].layerName, instance->layers[j]) == 0) {
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
			for (uint32_t j = 0; j < extensionCount; ++j) {
				if (strcmp(extensionsAvailable[i].extensionName, extensions[j]) == 0) {
					for (int k = 0; k < 50-strlen(extensionsAvailable[i].extensionName);k++) {
						printf(" ");
					}
					printf(" - Selected");
				}
			}
			printf("\n");
		}
	}

	VLKCheck(instance->pvkCreateInstance(&instanceInfo, instance->allocator, &instance->instance),
		"Failed to create Vulkan Instance");

	instance->pvkDestroyInstance = (PFN_vkDestroyInstance)instance->pvkGetInstanceProcAddr(instance->instance, "vkDestroyInstance");
	instance->pvkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)instance->pvkGetInstanceProcAddr(instance->instance, "vkEnumeratePhysicalDevices");
	instance->pvkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceFeatures");
	instance->pvkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceFormatProperties");
	instance->pvkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceImageFormatProperties");
	instance->pvkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceProperties");
	instance->pvkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceQueueFamilyProperties");
	instance->pvkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceMemoryProperties");
	instance->pvkCreateDevice = (PFN_vkCreateDevice)instance->pvkGetInstanceProcAddr(instance->instance, "vkCreateDevice");
	instance->pvkDestroyDevice = (PFN_vkDestroyDevice)instance->pvkGetInstanceProcAddr(instance->instance, "vkDestroyDevice");
	instance->pvkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkEnumerateDeviceExtensionProperties");
	instance->pvkEnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)instance->pvkGetInstanceProcAddr(instance->instance, "vkEnumerateDeviceLayerProperties");

	instance->pvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)instance->pvkGetInstanceProcAddr(instance->instance, "vkCreateDebugReportCallbackEXT");
	instance->pvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)instance->pvkGetInstanceProcAddr(instance->instance, "vkDestroyDebugReportCallbackEXT");

	instance->pvkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkDestroySurfaceKHR");
	instance->pvkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	instance->pvkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	instance->pvkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	instance->pvkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

	instance->pvkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");
	instance->pvkGetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
	instance->pvkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
	instance->pvkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetDisplayModePropertiesKHR");
	instance->pvkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkCreateDisplayModeKHR");
	instance->pvkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetDisplayPlaneCapabilitiesKHR");
	instance->pvkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)instance->pvkGetInstanceProcAddr(instance->instance, "vkCreateDisplayPlaneSurfaceKHR");

	if (debug) {
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		callbackCreateInfo.pNext = NULL;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = &__DebugReportCallback;
		callbackCreateInfo.pUserData = NULL;

		//VLKCheck(instance->pvkCreateDebugReportCallbackEXT(instance->instance, &callbackCreateInfo, instance->allocator, &instance->callback),
		//	"Could not create Debug Callback");
	}

	*pInstace = instance;

	return 0;
}

int vklDestroyInstance(VKLInstance* instance) {
	if (instance->debug) {
		//instance->pvkDestroyDebugReportCallbackEXT(instance->instance, instance->callback, instance->allocator);
	}

	instance->pvkDestroyInstance(instance->instance, instance->allocator);

	free_c(instance);

	return 0;
}

int vklDestroySurface(VKLInstance* instance, VKLSurface* surface) {
	instance->pvkDestroySurfaceKHR(instance->instance, surface->surface, instance->allocator);

	free_c(surface);
	
	return 0;
}
