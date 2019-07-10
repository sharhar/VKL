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
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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

	vklCreateFrameBuffer(context, &swapChain->backBuffer, swapChain->width, swapChain->height, colorFormat, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	swapChain->cmdBuffers = malloc_c(sizeof(VkCommandBuffer) * swapChain->imageCount);
	vklAllocateCommandBuffer(context, swapChain->cmdBuffers, VK_COMMAND_BUFFER_LEVEL_PRIMARY, swapChain->imageCount);

	for (uint32_t i = 0; i < swapChain->imageCount;i++) {
		vklBeginCommandBuffer(device, swapChain->cmdBuffers[i]);

		VkImageMemoryBarrier layoutTransitionBarrier;
		memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
		layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		layoutTransitionBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		layoutTransitionBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarrier.image = swapChain->presentImages[i];
		layoutTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		layoutTransitionBarrier.subresourceRange.baseMipLevel = 0;
		layoutTransitionBarrier.subresourceRange.levelCount = 1;
		layoutTransitionBarrier.subresourceRange.baseArrayLayer = 0;
		layoutTransitionBarrier.subresourceRange.layerCount = 1;

		device->pvkCmdPipelineBarrier(swapChain->cmdBuffers[i],
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, NULL,
			0, NULL,
			1, &layoutTransitionBarrier);

		VkImageSubresourceLayers subResource;
		memset(&subResource, 0, sizeof(VkImageSubresourceLayers));
		subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subResource.baseArrayLayer = 0;
		subResource.mipLevel = 0;
		subResource.layerCount = 1;

		VkImageCopy region;
		memset(&region, 0, sizeof(VkImageCopy));
		region.srcSubresource = subResource;
		region.dstSubresource = subResource;
		region.srcOffset.x = 0;
		region.srcOffset.y = 0;
		region.srcOffset.z = 0;
		region.dstOffset.x = 0;
		region.dstOffset.y = 0;
		region.dstOffset.z = 0;
		region.extent.width = swapChain->width;
		region.extent.height = swapChain->height;
		region.extent.depth = 1;

		device->pvkCmdCopyImage(
			swapChain->cmdBuffers[i],
			swapChain->backBuffer->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			swapChain->presentImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &region);

		memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
		layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		layoutTransitionBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		layoutTransitionBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarrier.image = swapChain->presentImages[i];
		layoutTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		layoutTransitionBarrier.subresourceRange.baseMipLevel = 0;
		layoutTransitionBarrier.subresourceRange.levelCount = 1;
		layoutTransitionBarrier.subresourceRange.baseArrayLayer = 0;
		layoutTransitionBarrier.subresourceRange.layerCount = 1;

		device->pvkCmdPipelineBarrier(swapChain->cmdBuffers[i],
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, NULL,
			0, NULL,
			1, &layoutTransitionBarrier);

		vklEndCommandBuffer(device, swapChain->cmdBuffers[i]);
	}

	*pSwapChain = swapChain;

	return 0;
}

int vklGetBackBuffer(VKLSwapChain* swapChain, VKLFrameBuffer** pFrameBuffer) {
	*pFrameBuffer = swapChain->backBuffer;

	return 0;
}

int vklDestroySwapChain(VKLSwapChain* swapChain) {
	VKLDevice* device = swapChain->context->device;

	vklDestroyFrameBuffer(device, swapChain->backBuffer);

	for (uint32_t i = 0; i < swapChain->imageCount;i++) {
		device->pvkDestroyImageView(device->device, swapChain->presentImageViews[i], device->instance->allocator);
	}

	swapChain->context->device->pvkDestroySwapchainKHR(swapChain->context->device->device, 
		swapChain->swapChain, swapChain->context->device->instance->allocator);

	free_c(swapChain);
	return 0;
}

int vklPresent(VKLSwapChain* swapChain) {
	VKLDevice* device = swapChain->context->device;

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;

	device->pvkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &swapChain->presentCompleteSemaphore);
	device->pvkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &swapChain->renderingCompleteSemaphore);

	device->pvkAcquireNextImageKHR(device->device, swapChain->swapChain, UINT64_MAX,
		swapChain->presentCompleteSemaphore, VK_NULL_HANDLE, &swapChain->nextImageIdx);

	VkPipelineStageFlags waitStageMash[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };

	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &swapChain->presentCompleteSemaphore;
	submitInfo.pWaitDstStageMask = waitStageMash;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &swapChain->cmdBuffers[swapChain->nextImageIdx];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &swapChain->renderingCompleteSemaphore;

	VLKCheck(device->pvkQueueSubmit(swapChain->context->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit Queue");
	
	VkPresentInfoKHR presentInfo;
	memset(&presentInfo, 0, sizeof(VkPresentInfoKHR));
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &swapChain->renderingCompleteSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain->swapChain;
	presentInfo.pImageIndices = &swapChain->nextImageIdx;
	presentInfo.pResults = NULL;
	device->pvkQueuePresentKHR(swapChain->context->queue, &presentInfo);

	device->pvkQueueWaitIdle(swapChain->context->queue);

	device->pvkResetCommandBuffer(swapChain->context->setupCmdBuffer, 0);

	device->pvkDestroySemaphore(device->device, swapChain->presentCompleteSemaphore, NULL);
	device->pvkDestroySemaphore(device->device, swapChain->renderingCompleteSemaphore, NULL);

	return 0;
}