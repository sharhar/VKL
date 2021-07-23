#include <VKL/VKL.h>

VKLDevice::VKLDevice(VKLDeviceCreateInfo* createInfo) {
	m_instance = createInfo->instance;
	m_physicalDevice = createInfo->physicalDevice;
	m_allocationCallbacks = m_instance->allocationCallbacks();

	vk.CreateDevice = (PFN_vkCreateDevice)m_instance->procAddr("vkCreateDevice");
	vk.GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)m_instance->procAddr("vkGetDeviceProcAddr");

	VkPhysicalDeviceFeatures features = m_physicalDevice->getFeatures();

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = NULL;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = createInfo->queueCreateInfo->typeCount;
	deviceCreateInfo.pQueueCreateInfos = createInfo->queueCreateInfo->createInfos;
	deviceCreateInfo.enabledLayerCount = m_instance->getLayers().size();
	deviceCreateInfo.ppEnabledLayerNames = (const char**)m_instance->getLayers().data();
	deviceCreateInfo.enabledExtensionCount = createInfo->extensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = createInfo->extensions.data();
	deviceCreateInfo.pEnabledFeatures = &features;

	VK_CALL(vk.CreateDevice(m_physicalDevice->handle(), &deviceCreateInfo, m_instance->allocationCallbacks(), &m_handle));

	vk.DestroyDevice = (PFN_vkDestroyDevice)procAddr("vkDestroyDevice");
	vk.GetDeviceQueue = (PFN_vkGetDeviceQueue)procAddr("vkGetDeviceQueue");
	vk.QueueSubmit = (PFN_vkQueueSubmit)procAddr("vkQueueSubmit");
	vk.QueueWaitIdle = (PFN_vkQueueWaitIdle)procAddr("vkQueueWaitIdle");
	vk.DeviceWaitIdle = (PFN_vkDeviceWaitIdle)procAddr("vkDeviceWaitIdle");
	vk.AllocateMemory = (PFN_vkAllocateMemory)procAddr("vkAllocateMemory");
	vk.FreeMemory = (PFN_vkFreeMemory)procAddr("vkFreeMemory");
	vk.MapMemory = (PFN_vkMapMemory)procAddr("vkMapMemory");
	vk.UnmapMemory = (PFN_vkUnmapMemory)procAddr("vkUnmapMemory");
	vk.FlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)procAddr("vkFlushMappedMemoryRanges");
	vk.InvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)procAddr("vkInvalidateMappedMemoryRanges");
	vk.GetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)procAddr("vkGetDeviceMemoryCommitment");
	vk.BindBufferMemory = (PFN_vkBindBufferMemory)procAddr("vkBindBufferMemory");
	vk.BindImageMemory = (PFN_vkBindImageMemory)procAddr("vkBindImageMemory");
	vk.GetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)procAddr("vkGetBufferMemoryRequirements");
	vk.GetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)procAddr("vkGetImageMemoryRequirements");
	vk.GetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)procAddr("vkGetImageSparseMemoryRequirements");
	vk.GetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)procAddr("vkGetPhysicalDeviceSparseImageFormatProperties");
	vk.QueueBindSparse = (PFN_vkQueueBindSparse)procAddr("vkQueueBindSparse");
	vk.CreateFence = (PFN_vkCreateFence)procAddr("vkCreateFence");
	vk.DestroyFence = (PFN_vkDestroyFence)procAddr("vkDestroyFence");
	vk.ResetFences = (PFN_vkResetFences)procAddr("vkResetFences");
	vk.GetFenceStatus = (PFN_vkGetFenceStatus)procAddr("vkGetFenceStatus");
	vk.WaitForFences = (PFN_vkWaitForFences)procAddr("vkWaitForFences");
	vk.CreateSemaphore = (PFN_vkCreateSemaphore)procAddr("vkCreateSemaphore");
	vk.DestroySemaphore = (PFN_vkDestroySemaphore)procAddr("vkDestroySemaphore");
	vk.CreateEvent = (PFN_vkCreateEvent)procAddr("vkCreateEvent");
	vk.DestroyEvent = (PFN_vkDestroyEvent)procAddr("vkDestroyEvent");
	vk.GetEventStatus = (PFN_vkGetEventStatus)procAddr("vkGetEventStatus");
	vk.SetEvent = (PFN_vkSetEvent)procAddr("vkSetEvent");
	vk.ResetEvent = (PFN_vkResetEvent)procAddr("vkResetEvent");
	vk.CreateQueryPool = (PFN_vkCreateQueryPool)procAddr("vkCreateQueryPool");
	vk.DestroyQueryPool = (PFN_vkDestroyQueryPool)procAddr("vkDestroyQueryPool");
	vk.GetQueryPoolResults = (PFN_vkGetQueryPoolResults)procAddr("vkGetQueryPoolResults");
	vk.CreateBuffer = (PFN_vkCreateBuffer)procAddr("vkCreateBuffer");
	vk.DestroyBuffer = (PFN_vkDestroyBuffer)procAddr("vkDestroyBuffer");
	vk.CreateBufferView = (PFN_vkCreateBufferView)procAddr("vkCreateBufferView");
	vk.DestroyBufferView = (PFN_vkDestroyBufferView)procAddr("vkDestroyBufferView");
	vk.CreateImage = (PFN_vkCreateImage)procAddr("vkCreateImage");
	vk.DestroyImage = (PFN_vkDestroyImage)procAddr("vkDestroyImage");
	vk.GetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)procAddr("vkGetImageSubresourceLayout");
	vk.CreateImageView = (PFN_vkCreateImageView)procAddr("vkCreateImageView");
	vk.DestroyImageView = (PFN_vkDestroyImageView)procAddr("vkDestroyImageView");
	vk.CreateShaderModule = (PFN_vkCreateShaderModule)procAddr("vkCreateShaderModule");
	vk.DestroyShaderModule = (PFN_vkDestroyShaderModule)procAddr("vkDestroyShaderModule");
	vk.CreatePipelineCache = (PFN_vkCreatePipelineCache)procAddr("vkCreatePipelineCache");
	vk.DestroyPipelineCache = (PFN_vkDestroyPipelineCache)procAddr("vkDestroyPipelineCache");
	vk.GetPipelineCacheData = (PFN_vkGetPipelineCacheData)procAddr("vkGetPipelineCacheData");
	vk.MergePipelineCaches = (PFN_vkMergePipelineCaches)procAddr("vkMergePipelineCaches");
	vk.CreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)procAddr("vkCreateGraphicsPipelines");
	vk.CreateComputePipelines = (PFN_vkCreateComputePipelines)procAddr("vkCreateComputePipelines");
	vk.DestroyPipeline = (PFN_vkDestroyPipeline)procAddr("vkDestroyPipeline");
	vk.CreatePipelineLayout = (PFN_vkCreatePipelineLayout)procAddr("vkCreatePipelineLayout");
	vk.DestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)procAddr("vkDestroyPipelineLayout");
	vk.CreateSampler = (PFN_vkCreateSampler)procAddr("vkCreateSampler");
	vk.DestroySampler = (PFN_vkDestroySampler)procAddr("vkDestroySampler");
	vk.CreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)procAddr("vkCreateDescriptorSetLayout");
	vk.DestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)procAddr("vkDestroyDescriptorSetLayout");
	vk.CreateDescriptorPool = (PFN_vkCreateDescriptorPool)procAddr("vkCreateDescriptorPool");
	vk.DestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)procAddr("vkDestroyDescriptorPool");
	vk.ResetDescriptorPool = (PFN_vkResetDescriptorPool)procAddr("vkResetDescriptorPool");
	vk.AllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)procAddr("vkAllocateDescriptorSets");
	vk.FreeDescriptorSets = (PFN_vkFreeDescriptorSets)procAddr("vkFreeDescriptorSets");
	vk.UpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)procAddr("vkUpdateDescriptorSets");
	vk.CreateFramebuffer = (PFN_vkCreateFramebuffer)procAddr("vkCreateFramebuffer");
	vk.DestroyFramebuffer = (PFN_vkDestroyFramebuffer)procAddr("vkDestroyFramebuffer");
	vk.CreateRenderPass = (PFN_vkCreateRenderPass)procAddr("vkCreateRenderPass");
	vk.DestroyRenderPass = (PFN_vkDestroyRenderPass)procAddr("vkDestroyRenderPass");
	vk.GetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)procAddr("vkGetRenderAreaGranularity");
	vk.CreateCommandPool = (PFN_vkCreateCommandPool)procAddr("vkCreateCommandPool");
	vk.DestroyCommandPool = (PFN_vkDestroyCommandPool)procAddr("vkDestroyCommandPool");
	vk.ResetCommandPool = (PFN_vkResetCommandPool)procAddr("vkResetCommandPool");
	vk.AllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)procAddr("vkAllocateCommandBuffers");
	vk.FreeCommandBuffers = (PFN_vkFreeCommandBuffers)procAddr("vkFreeCommandBuffers");
	vk.BeginCommandBuffer = (PFN_vkBeginCommandBuffer)procAddr("vkBeginCommandBuffer");
	vk.EndCommandBuffer = (PFN_vkEndCommandBuffer)procAddr("vkEndCommandBuffer");
	vk.ResetCommandBuffer = (PFN_vkResetCommandBuffer)procAddr("vkResetCommandBuffer");
	vk.CmdBindPipeline = (PFN_vkCmdBindPipeline)procAddr("vkCmdBindPipeline");
	vk.CmdSetViewport = (PFN_vkCmdSetViewport)procAddr("vkCmdSetViewport");
	vk.CmdSetScissor = (PFN_vkCmdSetScissor)procAddr("vkCmdSetScissor");
	vk.CmdSetLineWidth = (PFN_vkCmdSetLineWidth)procAddr("vkCmdSetLineWidth");
	vk.CmdSetDepthBias = (PFN_vkCmdSetDepthBias)procAddr("vkCmdSetDepthBias");
	vk.CmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)procAddr("vkCmdSetBlendConstants");
	vk.CmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)procAddr("vkCmdSetDepthBounds");
	vk.CmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)procAddr("vkCmdSetStencilCompareMask");
	vk.CmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)procAddr("vkCmdSetStencilWriteMask");
	vk.CmdSetStencilReference = (PFN_vkCmdSetStencilReference)procAddr("vkCmdSetStencilReference");
	vk.CmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)procAddr("vkCmdBindDescriptorSets");
	vk.CmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)procAddr("vkCmdBindIndexBuffer");
	vk.CmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)procAddr("vkCmdBindVertexBuffers");
	vk.CmdDraw = (PFN_vkCmdDraw)procAddr("vkCmdDraw");
	vk.CmdDrawIndexed = (PFN_vkCmdDrawIndexed)procAddr("vkCmdDrawIndexed");
	vk.CmdDrawIndirect = (PFN_vkCmdDrawIndirect)procAddr("vkCmdDrawIndirect");
	vk.CmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)procAddr("vkCmdDrawIndexedIndirect");
	vk.CmdDispatch = (PFN_vkCmdDispatch)procAddr("vkCmdDispatch");
	vk.CmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)procAddr("vkCmdDispatchIndirect");
	vk.CmdCopyBuffer = (PFN_vkCmdCopyBuffer)procAddr("vkCmdCopyBuffer");
	vk.CmdCopyImage = (PFN_vkCmdCopyImage)procAddr("vkCmdCopyImage");
	vk.CmdBlitImage = (PFN_vkCmdBlitImage)procAddr("vkCmdBlitImage");
	vk.CmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)procAddr("vkCmdCopyBufferToImage");
	vk.CmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)procAddr("vkCmdCopyImageToBuffer");
	vk.CmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)procAddr("vkCmdUpdateBuffer");
	vk.CmdFillBuffer = (PFN_vkCmdFillBuffer)procAddr("vkCmdFillBuffer");
	vk.CmdClearColorImage = (PFN_vkCmdClearColorImage)procAddr("vkCmdClearColorImage");
	vk.CmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)procAddr("vkCmdClearDepthStencilImage");
	vk.CmdClearAttachments = (PFN_vkCmdClearAttachments)procAddr("vkCmdClearAttachments");
	vk.CmdResolveImage = (PFN_vkCmdResolveImage)procAddr("vkCmdResolveImage");
	vk.CmdSetEvent = (PFN_vkCmdSetEvent)procAddr("vkCmdSetEvent");
	vk.CmdResetEvent = (PFN_vkCmdResetEvent)procAddr("vkCmdResetEvent");
	vk.CmdWaitEvents = (PFN_vkCmdWaitEvents)procAddr("vkCmdWaitEvents");
	vk.CmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)procAddr("vkCmdPipelineBarrier");
	vk.CmdBeginQuery = (PFN_vkCmdBeginQuery)procAddr("vkCmdBeginQuery");
	vk.CmdEndQuery = (PFN_vkCmdEndQuery)procAddr("vkCmdEndQuery");
	vk.CmdResetQueryPool = (PFN_vkCmdResetQueryPool)procAddr("vkCmdResetQueryPool");
	vk.CmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)procAddr("vkCmdWriteTimestamp");
	vk.CmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)procAddr("vkCmdCopyQueryPoolResults");
	vk.CmdPushConstants = (PFN_vkCmdPushConstants)procAddr("vkCmdPushConstants");
	vk.CmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)procAddr("vkCmdBeginRenderPass");
	vk.CmdNextSubpass = (PFN_vkCmdNextSubpass)procAddr("vkCmdNextSubpass");
	vk.CmdEndRenderPass = (PFN_vkCmdEndRenderPass)procAddr("vkCmdEndRenderPass");
	vk.CmdExecuteCommands = (PFN_vkCmdExecuteCommands)procAddr("vkCmdExecuteCommands");

	vk.CreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)procAddr("vkCreateSwapchainKHR");
	vk.DestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)procAddr("vkDestroySwapchainKHR");
	vk.GetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)procAddr("vkGetSwapchainImagesKHR");
	vk.AcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)procAddr("vkAcquireNextImageKHR");
	vk.QueuePresentKHR = (PFN_vkQueuePresentKHR)procAddr("vkQueuePresentKHR");

	vmaFuncs.vkGetPhysicalDeviceProperties = m_instance->vk.GetPhysicalDeviceProperties;
	vmaFuncs.vkGetPhysicalDeviceMemoryProperties = m_instance->vk.GetPhysicalDeviceMemoryProperties;

	vmaFuncs.vkAllocateMemory = vk.AllocateMemory;
	vmaFuncs.vkFreeMemory = vk.FreeMemory;
	vmaFuncs.vkMapMemory = vk.MapMemory;
	vmaFuncs.vkUnmapMemory = vk.UnmapMemory;
	vmaFuncs.vkFlushMappedMemoryRanges = vk.FlushMappedMemoryRanges;
	vmaFuncs.vkInvalidateMappedMemoryRanges = vk.InvalidateMappedMemoryRanges;
	vmaFuncs.vkBindBufferMemory = vk.BindBufferMemory;
	vmaFuncs.vkBindImageMemory = vk.BindImageMemory;
	vmaFuncs.vkGetBufferMemoryRequirements = vk.GetBufferMemoryRequirements;
	vmaFuncs.vkGetImageMemoryRequirements = vk.GetImageMemoryRequirements;
	vmaFuncs.vkCreateBuffer = vk.CreateBuffer;
	vmaFuncs.vkDestroyBuffer = vk.DestroyBuffer;
	vmaFuncs.vkCreateImage = vk.CreateImage;
	vmaFuncs.vkDestroyImage = vk.DestroyImage;
	vmaFuncs.vkCmdCopyBuffer = vk.CmdCopyBuffer;

	m_queuesCount = createInfo->queueCreateInfo->typeCount;
	m_queues = (VKLQueue**)malloc(sizeof(VKLQueue*) * m_queuesCount);
	m_queueTypeCount = (uint32_t*)malloc(sizeof(uint32_t) * m_queuesCount);
	
	for (int i = 0; i < m_queuesCount; i++) {
		uint32_t thisTypeCount = createInfo->queueCreateInfo->createInfos[i].queueCount;
		uint32_t familyIndex = createInfo->queueCreateInfo->createInfos[i].queueFamilyIndex;

		m_queueTypeCount[i] = thisTypeCount;
		m_queues[i] = new VKLQueue[thisTypeCount];

		for (int j = 0; j < thisTypeCount; j++) {
			VkQueue queue;
			vk.GetDeviceQueue(m_handle, familyIndex, j, &queue);
			m_queues[i][j].init(this, queue, familyIndex);
		}
	}
	
	VmaAllocatorCreateInfo vmaAllocatorCreateInfo;
	memset(&vmaAllocatorCreateInfo, 0, sizeof(VmaAllocatorCreateInfo));
	vmaAllocatorCreateInfo.instance = m_instance->handle();
	vmaAllocatorCreateInfo.physicalDevice = m_physicalDevice->handle();
	vmaAllocatorCreateInfo.device = m_handle;
	vmaAllocatorCreateInfo.pVulkanFunctions = &vmaFuncs;

	vmaCreateAllocator(&vmaAllocatorCreateInfo, &m_allocator);
}

VKLQueue& VKLDevice::getQueue(uint32_t typeIndex, uint32_t queueIndex) {
	return m_queues[typeIndex][queueIndex];
}

VkFence VKLDevice::createFence(VkFenceCreateFlags flags) {
	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = NULL;
	fenceCreateInfo.flags = flags;
	
	VkFence result;
	VK_CALL(vk.CreateFence(m_handle, &fenceCreateInfo, m_allocationCallbacks, &result));
	return result;
}

void VKLDevice::waitForFence(VkFence fence) {
	VK_CALL(vk.WaitForFences(m_handle, 1, &fence, VK_TRUE, UINT64_MAX));
}

void VKLDevice::resetFence(VkFence fence) {
	VK_CALL(vk.ResetFences(m_handle, 1, &fence));
}

void VKLDevice::destroyFence(VkFence fence) {
	VK_CALL(vk.DestroyFence(m_handle, fence, m_allocationCallbacks));
}

void VKLDevice::destroy() {
	for (int i = 0; i < m_queuesCount; i++) {
		for (int j = 0; j < m_queueTypeCount[i]; j++) {
			m_queues[i][j].getCmdBuffer()->destroy();
		}
		
		delete[] m_queues[i];
	}

	free(m_queueTypeCount);
	free(m_queues);
	
	vmaDestroyAllocator(m_allocator);

	vk.DestroyDevice(m_handle, allocationCallbacks());
}

VKLDeviceCreateInfo::VKLDeviceCreateInfo(VKLInstance* instance, VKLPhysicalDevice* physicalDevice, VKLQueueCreateInfo* queueCreateInfo) {
	this->instance = instance;
	this->physicalDevice = physicalDevice;
	this->queueCreateInfo = queueCreateInfo;
}

void VKLDeviceCreateInfo::addExtension(char* extension) {
	extensions.push_back(extension);
}
