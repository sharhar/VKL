#include <VKL/VKL.h>

int vklCreateSwapChain(VKLDeviceGraphicsContext* context, VKLSwapChain** pSwapChain, VkBool32 vSync) {
	VKLSwapChain* swapChain = (VKLSwapChain*)malloc_c(sizeof(VKLSwapChain));
	VKLDevice* device = context->device;

	swapChain->context = context;

	uint32_t formatCount = 0;
	device->instance->pvkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, context->surface->surface, &formatCount, NULL);
	VkSurfaceFormatKHR* surfaceFormats = malloc_c(sizeof(VkSurfaceFormatKHR) * formatCount);
	device->instance->pvkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, context->surface->surface, &formatCount, surfaceFormats);

	VkFormat colorFormat;
	if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		colorFormat = VK_FORMAT_B8G8R8_UNORM;
	}
	else {
		colorFormat = surfaceFormats[0].format;
	}

	VkColorSpaceKHR colorSpace;
	colorSpace = surfaceFormats[0].colorSpace;
	free_c(surfaceFormats);

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	memset(&surfaceCapabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));
	context->device->instance->pvkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->device->physicalDevice, context->surface->surface, &surfaceCapabilities);

	uint32_t desiredImageCount = 2;
	if (desiredImageCount < surfaceCapabilities.minImageCount) {
		desiredImageCount = surfaceCapabilities.minImageCount;
	}
	else if (surfaceCapabilities.maxImageCount != 0 &&
		desiredImageCount > surfaceCapabilities.maxImageCount) {
		desiredImageCount = surfaceCapabilities.maxImageCount;
	}

	swapChain->width = context->surface->width;
	swapChain->height = context->surface->height;

	VkExtent2D surfaceResolution = surfaceCapabilities.currentExtent;
	if (surfaceResolution.width == -1) {
		surfaceResolution.width = swapChain->width;
		surfaceResolution.height = swapChain->height;
	}
	else {
		swapChain->width = surfaceResolution.width;
		swapChain->height = surfaceResolution.height;
	}

	VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}

	uint32_t presentModeCount = 0;
	device->instance->pvkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, context->surface->surface, &presentModeCount, NULL);
	VkPresentModeKHR* presentModes = malloc_c(sizeof(VkPresentModeKHR) * presentModeCount);
	device->instance->pvkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, context->surface->surface, &presentModeCount, presentModes);

	VkPresentModeKHR presentationMode = VK_PRESENT_MODE_FIFO_KHR;
	if (!vSync) {
		for (uint32_t i = 0; i < presentModeCount; ++i) {
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
				presentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}
	}

	free_c(presentModes);

	VkSwapchainCreateInfoKHR swapChainCreateInfo;
	memset(&swapChainCreateInfo, 0, sizeof(VkSwapchainCreateInfoKHR));
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = context->surface->surface;
	swapChainCreateInfo.minImageCount = desiredImageCount;
	swapChainCreateInfo.imageFormat = colorFormat;
	swapChainCreateInfo.imageColorSpace = colorSpace;
	swapChainCreateInfo.imageExtent = surfaceResolution;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.preTransform = preTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = presentationMode;
	swapChainCreateInfo.clipped = VK_TRUE;

	VLKCheck(device->pvkCreateSwapchainKHR(device->device, &swapChainCreateInfo, device->instance->allocator, &swapChain->swapChain),
		"Failed to create swapchain");

	device->pvkGetSwapchainImagesKHR(device->device, swapChain->swapChain, &swapChain->imageCount, NULL);
	swapChain->presentImages = malloc_c(sizeof(VkImage) * swapChain->imageCount);
	device->pvkGetSwapchainImagesKHR(device->device, swapChain->swapChain, &swapChain->imageCount, swapChain->presentImages);

	VkImageViewCreateInfo presentImagesViewCreateInfo;
	memset(&presentImagesViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	presentImagesViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	presentImagesViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	presentImagesViewCreateInfo.format = colorFormat;
	presentImagesViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	presentImagesViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	presentImagesViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	presentImagesViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	presentImagesViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	presentImagesViewCreateInfo.subresourceRange.baseMipLevel = 0;
	presentImagesViewCreateInfo.subresourceRange.levelCount = 1;
	presentImagesViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	presentImagesViewCreateInfo.subresourceRange.layerCount = 1;

	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VkImageMemoryBarrier* layoutTransitionBarriers = malloc_c(sizeof(VkImageMemoryBarrier) * swapChain->imageCount);
	VkImageSubresourceRange resourceRange;
	memset(&resourceRange, 0, sizeof(VkImageSubresourceRange));
	resourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	resourceRange.baseMipLevel = 0;
	resourceRange.levelCount = 1;
	resourceRange.baseArrayLayer = 0;
	resourceRange.layerCount = 1;
	
	for (int i = 0; i < swapChain->imageCount; i++) {
		layoutTransitionBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		layoutTransitionBarriers[i].pNext = NULL;
		layoutTransitionBarriers[i].srcAccessMask = 0;
		layoutTransitionBarriers[i].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		layoutTransitionBarriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		layoutTransitionBarriers[i].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		layoutTransitionBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarriers[i].image = swapChain->presentImages[i];
		layoutTransitionBarriers[i].subresourceRange = resourceRange;
	}

	device->pvkBeginCommandBuffer(context->setupCmdBuffer, &beginInfo);

	device->pvkCmdPipelineBarrier(context->setupCmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		swapChain->imageCount, layoutTransitionBarriers);

	device->pvkEndCommandBuffer(context->setupCmdBuffer);

	VkPipelineStageFlags waitStageMash[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = waitStageMash;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context->setupCmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
	VLKCheck(device->pvkQueueSubmit(context->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit queue");

	device->pvkQueueWaitIdle(context->queue);

	swapChain->presentImageViews = malloc_c(sizeof(VkImageView) * swapChain->imageCount);
	for (uint32_t i = 0; i < swapChain->imageCount; ++i) {
		presentImagesViewCreateInfo.image = swapChain->presentImages[i];

		VLKCheck(device->pvkCreateImageView(device->device, &presentImagesViewCreateInfo, NULL, &swapChain->presentImageViews[i]),
			"Coud not create image view");
	}

	device->instance->pvkGetPhysicalDeviceMemoryProperties(device->physicalDevice, &device->memoryProperties);

	VkImageCreateInfo imageCreateInfo;
	memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageCreateInfo.extent.width = swapChain->width;
	imageCreateInfo.extent.height = swapChain->height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = NULL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VLKCheck(device->pvkCreateImage(device->device, &imageCreateInfo, NULL, &swapChain->depthImage),
		"Failed to create depth image");

	vklAllocateImageMemory(device, &swapChain->depthImageMemory, swapChain->depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	device->pvkBeginCommandBuffer(context->setupCmdBuffer, &beginInfo);

	VkImageMemoryBarrier layoutTransitionBarrier;
	memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = 0;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = swapChain->depthImage;
	layoutTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	layoutTransitionBarrier.subresourceRange.baseMipLevel = 0;
	layoutTransitionBarrier.subresourceRange.levelCount = 1;
	layoutTransitionBarrier.subresourceRange.baseArrayLayer = 0;
	layoutTransitionBarrier.subresourceRange.layerCount = 1;

	device->pvkCmdPipelineBarrier(context->setupCmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &layoutTransitionBarrier);

	device->pvkEndCommandBuffer(context->setupCmdBuffer);

	VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = waitStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context->setupCmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
	VLKCheck(device->pvkQueueSubmit(context->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit Queue");

	device->pvkQueueWaitIdle(context->queue);

	device->pvkResetCommandBuffer(context->setupCmdBuffer, 0);

	VkImageViewCreateInfo imageViewCreateInfo;
	memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = swapChain->depthImage;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = imageCreateInfo.format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VLKCheck(device->pvkCreateImageView(device->device, &imageViewCreateInfo, device->instance->allocator, &swapChain->depthImageView),
		"Failed to create image view");

	VkAttachmentDescription passAttachments[2];
	memset(passAttachments, 0, sizeof(VkAttachmentDescription) * 2);
	passAttachments[0].format = colorFormat;
	passAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	passAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	passAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	passAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	passAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	passAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	passAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	passAttachments[1].format = VK_FORMAT_D16_UNORM;
	passAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	passAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	passAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	passAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	passAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	passAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	passAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference;
	memset(&colorAttachmentReference, 0, sizeof(VkAttachmentReference));
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference;
	memset(&depthAttachmentReference, 0, sizeof(VkAttachmentReference));
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	memset(&subpass, 0, sizeof(VkSubpassDescription));
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	subpass.pDepthStencilAttachment = &depthAttachmentReference;

	VkRenderPassCreateInfo renderPassCreateInfo;
	memset(&renderPassCreateInfo, 0, sizeof(VkRenderPassCreateInfo));
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 2;
	renderPassCreateInfo.pAttachments = passAttachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;

	VLKCheck(device->pvkCreateRenderPass(device->device, &renderPassCreateInfo, device->instance->allocator, &swapChain->renderPass),
		"Failed to create renderpass");

	VkImageView frameBufferAttachments[2];
	frameBufferAttachments[1] = swapChain->depthImageView;

	VkFramebufferCreateInfo frameBufferCreateInfo;
	memset(&frameBufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.renderPass = swapChain->renderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = frameBufferAttachments;
	frameBufferCreateInfo.width = swapChain->width;
	frameBufferCreateInfo.height = swapChain->height;
	frameBufferCreateInfo.layers = 1;

	swapChain->frameBuffers = malloc_c(sizeof(VkFramebuffer) * swapChain->imageCount);
	for (uint32_t i = 0; i < swapChain->imageCount; ++i) {
		frameBufferAttachments[0] = swapChain->presentImageViews[i];
		VLKCheck(device->pvkCreateFramebuffer(device->device, &frameBufferCreateInfo, device->instance->allocator, &swapChain->frameBuffers[i]),
			"Failed to create framebuffer");
	}

	*pSwapChain = swapChain;

	return 0;
}

int vklDestroySwapChain(VKLSwapChain* swapChain) {
	VKLDevice* device = swapChain->context->device;

	for (uint32_t i = 0; i < swapChain->imageCount;i++) {
		device->pvkDestroyFramebuffer(device->device, swapChain->frameBuffers[i], device->instance->allocator);
		device->pvkDestroyImageView(device->device, swapChain->presentImageViews[i], device->instance->allocator);
	}

	device->pvkFreeMemory(device->device, swapChain->depthImageMemory, device->instance->allocator);
	device->pvkDestroyImageView(device->device, swapChain->depthImageView, device->instance->allocator);
	device->pvkDestroyImage(device->device, swapChain->depthImage, device->instance->allocator);

	device->pvkDestroyRenderPass(device->device, swapChain->renderPass, device->instance->allocator);

	swapChain->context->device->pvkDestroySwapchainKHR(swapChain->context->device->device, 
		swapChain->swapChain, swapChain->context->device->instance->allocator);

	free_c(swapChain);
	return 0;
}

int vklSetClearColor(VKLSwapChain* swapChain, float r, float g, float b, float a) {
	swapChain->clearR = r;
	swapChain->clearG = g;
	swapChain->clearB = b;
	swapChain->clearA = a;
	return 0;
}

int vklClearScreen(VKLSwapChain* swapChain) {
	VKLDevice* device = swapChain->context->device;

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;

	device->pvkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &swapChain->presentCompleteSemaphore);
	device->pvkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &swapChain->renderingCompleteSemaphore);

	VkResult result = device->pvkAcquireNextImageKHR(device->device, swapChain->swapChain, UINT64_MAX,
		swapChain->presentCompleteSemaphore, VK_NULL_HANDLE, &swapChain->nextImageIdx);

	swapChain->waitForRender = 0;

	return 0;
}

int vklBeginRenderRecording(VKLSwapChain* swapChain, VkCommandBuffer cmdBuffer) {
	VKLDevice* device = swapChain->context->device;

	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	device->pvkBeginCommandBuffer(cmdBuffer, &beginInfo);

	VkImageMemoryBarrier layoutTransitionBarrier;
	memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = swapChain->presentImages[swapChain->nextImageIdx];
	layoutTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	layoutTransitionBarrier.subresourceRange.baseMipLevel = 0;
	layoutTransitionBarrier.subresourceRange.levelCount = 1;
	layoutTransitionBarrier.subresourceRange.baseArrayLayer = 0;
	layoutTransitionBarrier.subresourceRange.layerCount = 1;

	device->pvkCmdPipelineBarrier(cmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &layoutTransitionBarrier);

	VkClearValue clearValue[] = {
		{ swapChain->clearR, swapChain->clearG, swapChain->clearB, swapChain->clearA },
		{ 1.0, 0.0 } };

	VkRenderPassBeginInfo renderPassBeginInfo;
	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = swapChain->renderPass;
	renderPassBeginInfo.framebuffer = swapChain->frameBuffers[swapChain->nextImageIdx];
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = swapChain->width;
	renderPassBeginInfo.renderArea.extent.height = swapChain->height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValue;
	device->pvkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	return 0;
}

int vklEndRenderRecording(VKLSwapChain* swapChain, VkCommandBuffer cmdBuffer) {
	VKLDevice* device = swapChain->context->device;

	device->pvkCmdEndRenderPass(cmdBuffer);

	VkImageMemoryBarrier prePresentBarrier;
	memset(&prePresentBarrier, 0, sizeof(VkImageMemoryBarrier));
	prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	prePresentBarrier.subresourceRange.baseMipLevel = 0;
	prePresentBarrier.subresourceRange.levelCount = 1;
	prePresentBarrier.subresourceRange.baseArrayLayer = 0;
	prePresentBarrier.subresourceRange.layerCount = 1;
	prePresentBarrier.image = swapChain->presentImages[swapChain->nextImageIdx];

	device->pvkCmdPipelineBarrier(cmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &prePresentBarrier);

	device->pvkEndCommandBuffer(cmdBuffer);

	return 0;
}

int vklRenderRecording(VKLSwapChain* swapChain, VkCommandBuffer cmdBuffer) {
	VKLDevice* device = swapChain->context->device;

	VkPipelineStageFlags waitStageMash = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &swapChain->presentCompleteSemaphore;
	submitInfo.pWaitDstStageMask = &waitStageMash;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &swapChain->renderingCompleteSemaphore;
	device->pvkQueueSubmit(swapChain->context->queue, 1, &submitInfo, VK_NULL_HANDLE);

	swapChain->waitForRender = 1;

	return 0;
}

int vklSwapBuffers(VKLSwapChain* swapChain) {
	VKLDevice* device = swapChain->context->device;

	VkPresentInfoKHR presentInfo;
	memset(&presentInfo, 0, sizeof(VkPresentInfoKHR));
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	
	if (swapChain->waitForRender) {
		presentInfo.pWaitSemaphores = &swapChain->renderingCompleteSemaphore;
	} else {
		presentInfo.pWaitSemaphores = &swapChain->presentCompleteSemaphore;
	}
	
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain->swapChain;
	presentInfo.pImageIndices = &swapChain->nextImageIdx;
	presentInfo.pResults = NULL;
	device->pvkQueuePresentKHR(swapChain->context->queue, &presentInfo);

	device->pvkQueueWaitIdle(swapChain->context->queue);

	device->pvkDestroySemaphore(device->device, swapChain->presentCompleteSemaphore, NULL);
	device->pvkDestroySemaphore(device->device, swapChain->renderingCompleteSemaphore, NULL);

	return 0;
}