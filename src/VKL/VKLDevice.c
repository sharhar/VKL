#include <VKL/VKL.h>

int vklCreateDevice(VKLInstance* instance, VKLDevice** pDevice, VKLSurface** pSurfaces,
	uint32_t deviceGraphicsContextCount, VKLDeviceGraphicsContext*** pDeviceGraphicsContexts,
	uint32_t deviceComputeContextCount, VKLDeviceComputeContext*** pDeviceComputeContexts) {

	*pDevice = (VKLDevice*)malloc_c(sizeof(VKLDevice));
	VKLDevice* device = *pDevice;
	device->deviceGraphicsContextCount = deviceGraphicsContextCount;
	device->deviceComputeContextCount = deviceComputeContextCount;
	device->instance = instance;
	device->physicalDevice = 0;

	if (deviceGraphicsContextCount > 0) {
		*pDeviceGraphicsContexts = (VKLDeviceGraphicsContext**)malloc_c(sizeof(VKLDeviceGraphicsContext*) * deviceGraphicsContextCount);
		
		device->deviceGraphicsContexts = *pDeviceGraphicsContexts;

		for (int i = 0; i < deviceGraphicsContextCount; i++) {
			device->deviceGraphicsContexts[i] = (VKLDeviceGraphicsContext*)malloc_c(sizeof(VKLDeviceGraphicsContext));

			device->deviceGraphicsContexts[i]->device = device;
			device->deviceGraphicsContexts[i]->surface = pSurfaces[i];
		}
	}

	if (deviceComputeContextCount > 0) {
		*pDeviceComputeContexts = (VKLDeviceComputeContext**)malloc_c(sizeof(VKLDeviceComputeContext*) * deviceComputeContextCount);
		device->deviceComputeContexts = *pDeviceComputeContexts;

		for (int i = 0; i < deviceComputeContextCount; i++) {
			device->deviceComputeContexts[i] = (VKLDeviceComputeContext*)malloc_c(sizeof(VKLDeviceComputeContext));

			device->deviceComputeContexts[i]->device = device;
		}
	}

	uint32_t physicalDeviceCount = 0;
	instance->pvkEnumeratePhysicalDevices(instance->instance, &physicalDeviceCount, NULL);
	VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc_c(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	instance->pvkEnumeratePhysicalDevices(instance->instance, &physicalDeviceCount, physicalDevices);

	uint32_t graphicsQueueIndex = -1;
	uint32_t computeQueueIndex = -1;

	for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties deviceProperties;
		instance->pvkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

		uint32_t queueFamilyCount = 0;
		instance->pvkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, NULL);
		VkQueueFamilyProperties* queueFamilyProperties = (VkQueueFamilyProperties*)malloc_c(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
		instance->pvkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i],
			&queueFamilyCount,
			queueFamilyProperties);

		graphicsQueueIndex = -1;
		computeQueueIndex = -1;

		for (uint32_t j = 0; j < queueFamilyCount; ++j) {
			if (deviceGraphicsContextCount > 0) {
				uint8_t supportsSurfaces = 1;
				for (int k = 0; k < deviceGraphicsContextCount; k++) {
					VkBool32 supportsPresent;
					instance->pvkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, device->deviceGraphicsContexts[k]->surface->surface, &supportsPresent);
					if (!supportsPresent) {
						supportsSurfaces = 0;
						break;
					}
				
				}

				if (supportsSurfaces && (queueFamilyProperties[j].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))) {
					graphicsQueueIndex = j;
				}
			}

			if (deviceComputeContextCount > 0) {
				if (queueFamilyProperties[j].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)) {
					computeQueueIndex = j;
				}
			}

			if ((graphicsQueueIndex != -1 || deviceGraphicsContextCount == 0) && (computeQueueIndex != -1 || deviceComputeContextCount == 0)) {
				break;
			}
		}

		free(queueFamilyProperties);

		if ((graphicsQueueIndex != -1 || deviceGraphicsContextCount == 0) && (computeQueueIndex != -1 || deviceComputeContextCount == 0)) {
			device->physicalDevice = physicalDevices[i];
			device->physicalDeviceProperties = deviceProperties;	

			break;
		}
	}

	free_c(physicalDevices);

	if (device->physicalDevice == 0) {
		return -1;
	}
	
	if (deviceGraphicsContextCount > 0) {
		for (int i = 0; i < deviceGraphicsContextCount; i++) {
			device->deviceGraphicsContexts[i]->queueIdx = graphicsQueueIndex;
		}
	}

	if (deviceComputeContextCount > 0) {
		for (int i = 0; i < deviceComputeContextCount; i++) {
			device->deviceComputeContexts[i]->queueIdx = computeQueueIndex;
		}
	}

	uint32_t queueCreateCount = 0;
	VkDeviceQueueCreateInfo* queueCreateInfos = NULL;

	if (deviceGraphicsContextCount > 0 && deviceComputeContextCount > 0 && graphicsQueueIndex == computeQueueIndex) {
		queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc_c(sizeof(VkDeviceQueueCreateInfo) * 1);
		queueCreateCount = 1;

		float* queuePriorities = (float*)malloc_c(sizeof(float) * (deviceGraphicsContextCount + deviceComputeContextCount));
		for (int i = 0; i < deviceGraphicsContextCount + deviceComputeContextCount; i++) {
			queuePriorities[i] = 1.0f;
		}

		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].pNext = NULL;
		queueCreateInfos[0].flags = 0;
		queueCreateInfos[0].queueFamilyIndex = graphicsQueueIndex;
		queueCreateInfos[0].queueCount = deviceGraphicsContextCount + deviceComputeContextCount;
		queueCreateInfos[0].pQueuePriorities = queuePriorities;
	} else if (deviceGraphicsContextCount > 0 && deviceComputeContextCount > 0 && graphicsQueueIndex != computeQueueIndex) {
		queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc_c(sizeof(VkDeviceQueueCreateInfo) * 2);
		queueCreateCount = 2;

		float* graphicsQueuePriorities = (float*)malloc_c(sizeof(float) * deviceGraphicsContextCount);
		for (int i = 0; i < deviceGraphicsContextCount; i++) {
			graphicsQueuePriorities[i] = 1.0f;
		}

		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].pNext = NULL;
		queueCreateInfos[0].flags = 0;
		queueCreateInfos[0].queueFamilyIndex = graphicsQueueIndex;
		queueCreateInfos[0].queueCount = deviceGraphicsContextCount;
		queueCreateInfos[0].pQueuePriorities = graphicsQueuePriorities;

		float* computeQueuePriorities = (float*)malloc_c(sizeof(float) * deviceComputeContextCount);
		for (int i = 0; i < deviceComputeContextCount; i++) {
			computeQueuePriorities[i] = 1.0f;
		}

		queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[1].pNext = NULL;
		queueCreateInfos[1].flags = 0;
		queueCreateInfos[1].queueFamilyIndex = computeQueueIndex;
		queueCreateInfos[1].queueCount = deviceComputeContextCount;
		queueCreateInfos[1].pQueuePriorities = computeQueuePriorities;

	} else if (deviceGraphicsContextCount > 0 && deviceComputeContextCount == 0) {
		queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc_c(sizeof(VkDeviceQueueCreateInfo) * 1);
		queueCreateCount = 1;

		float* graphicsQueuePriorities = (float*)malloc_c(sizeof(float) * deviceGraphicsContextCount);
		for (int i = 0; i < deviceGraphicsContextCount; i++) {
			graphicsQueuePriorities[i] = 1.0f;
		}

		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].pNext = NULL;
		queueCreateInfos[0].flags = 0;
		queueCreateInfos[0].queueFamilyIndex = graphicsQueueIndex;
		queueCreateInfos[0].queueCount = deviceGraphicsContextCount;
		queueCreateInfos[0].pQueuePriorities = graphicsQueuePriorities;

	} else if (deviceGraphicsContextCount == 0 && deviceComputeContextCount > 0) {
		queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc_c(sizeof(VkDeviceQueueCreateInfo) * 1);
		queueCreateCount = 1;

		float* computeQueuePriorities = (float*)malloc_c(sizeof(float) * deviceComputeContextCount);
		for (int i = 0; i < deviceComputeContextCount; i++) {
			computeQueuePriorities[i] = 1.0f;
		}

		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].pNext = NULL;
		queueCreateInfos[0].flags = 0;
		queueCreateInfos[0].queueFamilyIndex = computeQueueIndex;
		queueCreateInfos[0].queueCount = deviceComputeContextCount;
		queueCreateInfos[0].pQueuePriorities = computeQueuePriorities;
	}

	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = NULL;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = queueCreateCount;
	deviceInfo.pQueueCreateInfos = queueCreateInfos;
	deviceInfo.enabledLayerCount = instance->layerCount;
	deviceInfo.ppEnabledLayerNames = (const char**)instance->layers;

	const char *deviceExtensions[] = { "VK_KHR_swapchain" };
	deviceInfo.enabledExtensionCount = 1;
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;

	VkPhysicalDeviceFeatures features;
	instance->pvkGetPhysicalDeviceFeatures(device->physicalDevice, &features);

	if (features.shaderClipDistance != VK_TRUE || features.dualSrcBlend != VK_TRUE) {
		printf("Device does not support required features!\n");
		return -1;
	}

	deviceInfo.pEnabledFeatures = &features;

	VLKCheck(instance->pvkCreateDevice(device->physicalDevice, &deviceInfo, device->instance->allocator, &device->device),
		"Failed to create logical device");

	device->pvkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)instance->pvkGetInstanceProcAddr(instance->instance, "vkGetDeviceProcAddr");

	device->pvkGetDeviceQueue = (PFN_vkGetDeviceQueue)device->pvkGetDeviceProcAddr(device->device, "vkGetDeviceQueue");
	device->pvkQueueSubmit = (PFN_vkQueueSubmit)device->pvkGetDeviceProcAddr(device->device, "vkQueueSubmit");
	device->pvkQueueWaitIdle = (PFN_vkQueueWaitIdle)device->pvkGetDeviceProcAddr(device->device, "vkQueueWaitIdle");
	device->pvkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)device->pvkGetDeviceProcAddr(device->device, "vkDeviceWaitIdle");
	device->pvkAllocateMemory = (PFN_vkAllocateMemory)device->pvkGetDeviceProcAddr(device->device, "vkAllocateMemory");
	device->pvkFreeMemory = (PFN_vkFreeMemory)device->pvkGetDeviceProcAddr(device->device, "vkFreeMemory");
	device->pvkMapMemory = (PFN_vkMapMemory)device->pvkGetDeviceProcAddr(device->device, "vkMapMemory");
	device->pvkUnmapMemory = (PFN_vkUnmapMemory)device->pvkGetDeviceProcAddr(device->device, "vkUnmapMemory");
	device->pvkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)device->pvkGetDeviceProcAddr(device->device, "vkFlushMappedMemoryRanges");
	device->pvkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)device->pvkGetDeviceProcAddr(device->device, "vkInvalidateMappedMemoryRanges");
	device->pvkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)device->pvkGetDeviceProcAddr(device->device, "vkGetDeviceMemoryCommitment");
	device->pvkBindBufferMemory = (PFN_vkBindBufferMemory)device->pvkGetDeviceProcAddr(device->device, "vkBindBufferMemory");
	device->pvkBindImageMemory = (PFN_vkBindImageMemory)device->pvkGetDeviceProcAddr(device->device, "vkBindImageMemory");
	device->pvkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)device->pvkGetDeviceProcAddr(device->device, "vkGetBufferMemoryRequirements");
	device->pvkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)device->pvkGetDeviceProcAddr(device->device, "vkGetImageMemoryRequirements");
	device->pvkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)device->pvkGetDeviceProcAddr(device->device, "vkGetImageSparseMemoryRequirements");
	device->pvkGetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)device->pvkGetDeviceProcAddr(device->device, "vkGetPhysicalDeviceSparseImageFormatProperties");
	device->pvkQueueBindSparse = (PFN_vkQueueBindSparse)device->pvkGetDeviceProcAddr(device->device, "vkQueueBindSparse");
	device->pvkCreateFence = (PFN_vkCreateFence)device->pvkGetDeviceProcAddr(device->device, "vkCreateFence");
	device->pvkDestroyFence = (PFN_vkDestroyFence)device->pvkGetDeviceProcAddr(device->device, "vkDestroyFence");
	device->pvkResetFences = (PFN_vkResetFences)device->pvkGetDeviceProcAddr(device->device, "vkResetFences");
	device->pvkGetFenceStatus = (PFN_vkGetFenceStatus)device->pvkGetDeviceProcAddr(device->device, "vkGetFenceStatus");
	device->pvkWaitForFences = (PFN_vkWaitForFences)device->pvkGetDeviceProcAddr(device->device, "vkWaitForFences");
	device->pvkCreateSemaphore = (PFN_vkCreateSemaphore)device->pvkGetDeviceProcAddr(device->device, "vkCreateSemaphore");
	device->pvkDestroySemaphore = (PFN_vkDestroySemaphore)device->pvkGetDeviceProcAddr(device->device, "vkDestroySemaphore");
	device->pvkCreateEvent = (PFN_vkCreateEvent)device->pvkGetDeviceProcAddr(device->device, "vkCreateEvent");
	device->pvkDestroyEvent = (PFN_vkDestroyEvent)device->pvkGetDeviceProcAddr(device->device, "vkDestroyEvent");
	device->pvkGetEventStatus = (PFN_vkGetEventStatus)device->pvkGetDeviceProcAddr(device->device, "vkGetEventStatus");
	device->pvkSetEvent = (PFN_vkSetEvent)device->pvkGetDeviceProcAddr(device->device, "vkSetEvent");
	device->pvkResetEvent = (PFN_vkResetEvent)device->pvkGetDeviceProcAddr(device->device, "vkResetEvent");
	device->pvkCreateQueryPool = (PFN_vkCreateQueryPool)device->pvkGetDeviceProcAddr(device->device, "vkCreateQueryPool");
	device->pvkDestroyQueryPool = (PFN_vkDestroyQueryPool)device->pvkGetDeviceProcAddr(device->device, "vkDestroyQueryPool");
	device->pvkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)device->pvkGetDeviceProcAddr(device->device, "vkGetQueryPoolResults");
	device->pvkCreateBuffer = (PFN_vkCreateBuffer)device->pvkGetDeviceProcAddr(device->device, "vkCreateBuffer");
	device->pvkDestroyBuffer = (PFN_vkDestroyBuffer)device->pvkGetDeviceProcAddr(device->device, "vkDestroyBuffer");
	device->pvkCreateBufferView = (PFN_vkCreateBufferView)device->pvkGetDeviceProcAddr(device->device, "vkCreateBufferView");
	device->pvkDestroyBufferView = (PFN_vkDestroyBufferView)device->pvkGetDeviceProcAddr(device->device, "vkDestroyBufferView");
	device->pvkCreateImage = (PFN_vkCreateImage)device->pvkGetDeviceProcAddr(device->device, "vkCreateImage");
	device->pvkDestroyImage = (PFN_vkDestroyImage)device->pvkGetDeviceProcAddr(device->device, "vkDestroyImage");
	device->pvkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)device->pvkGetDeviceProcAddr(device->device, "vkGetImageSubresourceLayout");
	device->pvkCreateImageView = (PFN_vkCreateImageView)device->pvkGetDeviceProcAddr(device->device, "vkCreateImageView");
	device->pvkDestroyImageView = (PFN_vkDestroyImageView)device->pvkGetDeviceProcAddr(device->device, "vkDestroyImageView");
	device->pvkCreateShaderModule = (PFN_vkCreateShaderModule)device->pvkGetDeviceProcAddr(device->device, "vkCreateShaderModule");
	device->pvkDestroyShaderModule = (PFN_vkDestroyShaderModule)device->pvkGetDeviceProcAddr(device->device, "vkDestroyShaderModule");
	device->pvkCreatePipelineCache = (PFN_vkCreatePipelineCache)device->pvkGetDeviceProcAddr(device->device, "vkCreatePipelineCache");
	device->pvkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)device->pvkGetDeviceProcAddr(device->device, "vkDestroyPipelineCache");
	device->pvkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)device->pvkGetDeviceProcAddr(device->device, "vkGetPipelineCacheData");
	device->pvkMergePipelineCaches = (PFN_vkMergePipelineCaches)device->pvkGetDeviceProcAddr(device->device, "vkMergePipelineCaches");
	device->pvkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)device->pvkGetDeviceProcAddr(device->device, "vkCreateGraphicsPipelines");
	device->pvkCreateComputePipelines = (PFN_vkCreateComputePipelines)device->pvkGetDeviceProcAddr(device->device, "vkCreateComputePipelines");
	device->pvkDestroyPipeline = (PFN_vkDestroyPipeline)device->pvkGetDeviceProcAddr(device->device, "vkDestroyPipeline");
	device->pvkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)device->pvkGetDeviceProcAddr(device->device, "vkCreatePipelineLayout");
	device->pvkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)device->pvkGetDeviceProcAddr(device->device, "vkDestroyPipelineLayout");
	device->pvkCreateSampler = (PFN_vkCreateSampler)device->pvkGetDeviceProcAddr(device->device, "vkCreateSampler");
	device->pvkDestroySampler = (PFN_vkDestroySampler)device->pvkGetDeviceProcAddr(device->device, "vkDestroySampler");
	device->pvkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)device->pvkGetDeviceProcAddr(device->device, "vkCreateDescriptorSetLayout");
	device->pvkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)device->pvkGetDeviceProcAddr(device->device, "vkDestroyDescriptorSetLayout");
	device->pvkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)device->pvkGetDeviceProcAddr(device->device, "vkCreateDescriptorPool");
	device->pvkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)device->pvkGetDeviceProcAddr(device->device, "vkDestroyDescriptorPool");
	device->pvkResetDescriptorPool = (PFN_vkResetDescriptorPool)device->pvkGetDeviceProcAddr(device->device, "vkResetDescriptorPool");
	device->pvkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)device->pvkGetDeviceProcAddr(device->device, "vkAllocateDescriptorSets");
	device->pvkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)device->pvkGetDeviceProcAddr(device->device, "vkFreeDescriptorSets");
	device->pvkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)device->pvkGetDeviceProcAddr(device->device, "vkUpdateDescriptorSets");
	device->pvkCreateFramebuffer = (PFN_vkCreateFramebuffer)device->pvkGetDeviceProcAddr(device->device, "vkCreateFramebuffer");
	device->pvkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)device->pvkGetDeviceProcAddr(device->device, "vkDestroyFramebuffer");
	device->pvkCreateRenderPass = (PFN_vkCreateRenderPass)device->pvkGetDeviceProcAddr(device->device, "vkCreateRenderPass");
	device->pvkDestroyRenderPass = (PFN_vkDestroyRenderPass)device->pvkGetDeviceProcAddr(device->device, "vkDestroyRenderPass");
	device->pvkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)device->pvkGetDeviceProcAddr(device->device, "vkGetRenderAreaGranularity");
	device->pvkCreateCommandPool = (PFN_vkCreateCommandPool)device->pvkGetDeviceProcAddr(device->device, "vkCreateCommandPool");
	device->pvkDestroyCommandPool = (PFN_vkDestroyCommandPool)device->pvkGetDeviceProcAddr(device->device, "vkDestroyCommandPool");
	device->pvkResetCommandPool = (PFN_vkResetCommandPool)device->pvkGetDeviceProcAddr(device->device, "vkResetCommandPool");
	device->pvkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)device->pvkGetDeviceProcAddr(device->device, "vkAllocateCommandBuffers");
	device->pvkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)device->pvkGetDeviceProcAddr(device->device, "vkFreeCommandBuffers");
	device->pvkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)device->pvkGetDeviceProcAddr(device->device, "vkBeginCommandBuffer");
	device->pvkEndCommandBuffer = (PFN_vkEndCommandBuffer)device->pvkGetDeviceProcAddr(device->device, "vkEndCommandBuffer");
	device->pvkResetCommandBuffer = (PFN_vkResetCommandBuffer)device->pvkGetDeviceProcAddr(device->device, "vkResetCommandBuffer");
	device->pvkCmdBindPipeline = (PFN_vkCmdBindPipeline)device->pvkGetDeviceProcAddr(device->device, "vkCmdBindPipeline");
	device->pvkCmdSetViewport = (PFN_vkCmdSetViewport)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetViewport");
	device->pvkCmdSetScissor = (PFN_vkCmdSetScissor)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetScissor");
	device->pvkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetLineWidth");
	device->pvkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetDepthBias");
	device->pvkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetBlendConstants");
	device->pvkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetDepthBounds");
	device->pvkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetStencilCompareMask");
	device->pvkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetStencilWriteMask");
	device->pvkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetStencilReference");
	device->pvkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)device->pvkGetDeviceProcAddr(device->device, "vkCmdBindDescriptorSets");
	device->pvkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)device->pvkGetDeviceProcAddr(device->device, "vkCmdBindIndexBuffer");
	device->pvkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)device->pvkGetDeviceProcAddr(device->device, "vkCmdBindVertexBuffers");
	device->pvkCmdDraw = (PFN_vkCmdDraw)device->pvkGetDeviceProcAddr(device->device, "vkCmdDraw");
	device->pvkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)device->pvkGetDeviceProcAddr(device->device, "vkCmdDrawIndexed");
	device->pvkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)device->pvkGetDeviceProcAddr(device->device, "vkCmdDrawIndirect");
	device->pvkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)device->pvkGetDeviceProcAddr(device->device, "vkCmdDrawIndexedIndirect");
	device->pvkCmdDispatch = (PFN_vkCmdDispatch)device->pvkGetDeviceProcAddr(device->device, "vkCmdDispatch");
	device->pvkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)device->pvkGetDeviceProcAddr(device->device, "vkCmdDispatchIndirect");
	device->pvkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)device->pvkGetDeviceProcAddr(device->device, "vkCmdCopyBuffer");
	device->pvkCmdCopyImage = (PFN_vkCmdCopyImage)device->pvkGetDeviceProcAddr(device->device, "vkCmdCopyImage");
	device->pvkCmdBlitImage = (PFN_vkCmdBlitImage)device->pvkGetDeviceProcAddr(device->device, "vkCmdBlitImage");
	device->pvkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)device->pvkGetDeviceProcAddr(device->device, "vkCmdCopyBufferToImage");
	device->pvkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)device->pvkGetDeviceProcAddr(device->device, "vkCmdCopyImageToBuffer");
	device->pvkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)device->pvkGetDeviceProcAddr(device->device, "vkCmdUpdateBuffer");
	device->pvkCmdFillBuffer = (PFN_vkCmdFillBuffer)device->pvkGetDeviceProcAddr(device->device, "vkCmdFillBuffer");
	device->pvkCmdClearColorImage = (PFN_vkCmdClearColorImage)device->pvkGetDeviceProcAddr(device->device, "vkCmdClearColorImage");
	device->pvkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)device->pvkGetDeviceProcAddr(device->device, "vkCmdClearDepthStencilImage");
	device->pvkCmdClearAttachments = (PFN_vkCmdClearAttachments)device->pvkGetDeviceProcAddr(device->device, "vkCmdClearAttachments");
	device->pvkCmdResolveImage = (PFN_vkCmdResolveImage)device->pvkGetDeviceProcAddr(device->device, "vkCmdResolveImage");
	device->pvkCmdSetEvent = (PFN_vkCmdSetEvent)device->pvkGetDeviceProcAddr(device->device, "vkCmdSetEvent");
	device->pvkCmdResetEvent = (PFN_vkCmdResetEvent)device->pvkGetDeviceProcAddr(device->device, "vkCmdResetEvent");
	device->pvkCmdWaitEvents = (PFN_vkCmdWaitEvents)device->pvkGetDeviceProcAddr(device->device, "vkCmdWaitEvents");
	device->pvkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)device->pvkGetDeviceProcAddr(device->device, "vkCmdPipelineBarrier");
	device->pvkCmdBeginQuery = (PFN_vkCmdBeginQuery)device->pvkGetDeviceProcAddr(device->device, "vkCmdBeginQuery");
	device->pvkCmdEndQuery = (PFN_vkCmdEndQuery)device->pvkGetDeviceProcAddr(device->device, "vkCmdEndQuery");
	device->pvkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)device->pvkGetDeviceProcAddr(device->device, "vkCmdResetQueryPool");
	device->pvkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)device->pvkGetDeviceProcAddr(device->device, "vkCmdWriteTimestamp");
	device->pvkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)device->pvkGetDeviceProcAddr(device->device, "vkCmdCopyQueryPoolResults");
	device->pvkCmdPushConstants = (PFN_vkCmdPushConstants)device->pvkGetDeviceProcAddr(device->device, "vkCmdPushConstants");
	device->pvkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)device->pvkGetDeviceProcAddr(device->device, "vkCmdBeginRenderPass");
	device->pvkCmdNextSubpass = (PFN_vkCmdNextSubpass)device->pvkGetDeviceProcAddr(device->device, "vkCmdNextSubpass");
	device->pvkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)device->pvkGetDeviceProcAddr(device->device, "vkCmdEndRenderPass");
	device->pvkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)device->pvkGetDeviceProcAddr(device->device, "vkCmdExecuteCommands");

	device->pvkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)device->pvkGetDeviceProcAddr(device->device, "vkCreateSwapchainKHR");
	device->pvkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)device->pvkGetDeviceProcAddr(device->device, "vkDestroySwapchainKHR");
	device->pvkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)device->pvkGetDeviceProcAddr(device->device, "vkGetSwapchainImagesKHR");
	device->pvkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)device->pvkGetDeviceProcAddr(device->device, "vkAcquireNextImageKHR");
	device->pvkQueuePresentKHR = (PFN_vkQueuePresentKHR)device->pvkGetDeviceProcAddr(device->device, "vkQueuePresentKHR");

	VkCommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = NULL;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = graphicsQueueIndex;

	for (int i = 0; i < deviceGraphicsContextCount; i++) {
		device->pvkGetDeviceQueue(device->device, graphicsQueueIndex, i, &device->deviceGraphicsContexts[i]->queue);
		VLKCheck(device->pvkCreateCommandPool(device->device, &commandPoolCreateInfo, device->instance->allocator, &device->deviceGraphicsContexts[i]->commandPool),
			"Failed to create command pool");
			
		vklAllocateCommandBuffer(device->deviceGraphicsContexts[i], &device->deviceGraphicsContexts[i]->setupCmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	}

	commandPoolCreateInfo.queueFamilyIndex = computeQueueIndex;

	for (int i = 0; i < deviceComputeContextCount; i++) {
		device->pvkGetDeviceQueue(device->device, computeQueueIndex, i, &device->deviceComputeContexts[i]->queue);
		VLKCheck(device->pvkCreateCommandPool(device->device, &commandPoolCreateInfo, device->instance->allocator, &device->deviceComputeContexts[i]->commandPool),
			"Failed to create command pool");

		vklAllocateCommandBuffer(device->deviceComputeContexts[i], &device->deviceComputeContexts[i]->setupCmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	}

	device->instance->pvkGetPhysicalDeviceMemoryProperties(device->physicalDevice, &device->memoryProperties);

	return 0;
}

int vklDestroyDevice(VKLDevice* device) {
	for (int i = 0; i < device->deviceGraphicsContextCount;i ++) {
		device->pvkDestroyCommandPool(device->device, device->deviceGraphicsContexts[i]->commandPool, device->instance->allocator);

		free_c(device->deviceGraphicsContexts[i]);
	}

	for (int i = 0; i < device->deviceComputeContextCount; i++) {
		device->pvkDestroyCommandPool(device->device, device->deviceComputeContexts[i]->commandPool, device->instance->allocator);

		free_c(device->deviceComputeContexts[i]);
	}

	device->instance->pvkDestroyDevice(device->device, device->instance->allocator);

	free_c(device);

	return 0;
}

int vklAllocateCommandBuffer(VKLDeviceGraphicsContext* context, VkCommandBuffer* cmdBuffer, VkCommandBufferLevel level, uint32_t count) {
	VkCommandBufferAllocateInfo commandBufferAllocationInfo;
	memset(&commandBufferAllocationInfo, 0, sizeof(VkCommandBufferAllocateInfo));
	commandBufferAllocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocationInfo.commandPool = context->commandPool;
	commandBufferAllocationInfo.level = level;
	commandBufferAllocationInfo.commandBufferCount = count;

	VLKCheck(context->device->pvkAllocateCommandBuffers(context->device->device, &commandBufferAllocationInfo, cmdBuffer),
		"Failed to allocate setup command buffer");

	return 0;
}

int vklBeginCommandBuffer(VKLDevice* device, VkCommandBuffer cmdBuffer) {
	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	device->pvkBeginCommandBuffer(cmdBuffer, &beginInfo);
}

int vklEndCommandBuffer(VKLDevice* device, VkCommandBuffer cmdBuffer) {
	device->pvkEndCommandBuffer(cmdBuffer);
}

int vklExecuteCommandBuffer(VKLDeviceGraphicsContext* devCon, VkCommandBuffer cmdBuffer) {
	VKLDevice* device = devCon->device;

	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
	device->pvkQueueSubmit(devCon->queue, 1, &submitInfo, VK_NULL_HANDLE);

	device->pvkQueueWaitIdle(devCon->queue);

	return 0;
}
