#include <VKL/VKL.h>

int vklCreateFrameBuffer(VKLDeviceGraphicsContext* devCon, VKLFrameBuffer** pFrameBuffer, uint32_t width, uint32_t height, VkFormat imageFormat, VkAccessFlags accessMask, VkImageLayout layout) {
	VKLFrameBuffer* frameBuffer = malloc_c(sizeof(VKLFrameBuffer));
	VKLDevice* device = devCon->device;

	frameBuffer->width = width;
	frameBuffer->height = height;
	frameBuffer->accessMask = accessMask;
	frameBuffer->layout = layout;

	VkImageCreateInfo imageCreateInfo;
	memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = imageFormat;
	imageCreateInfo.extent.width = frameBuffer->width;
	imageCreateInfo.extent.height = frameBuffer->height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = NULL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VLKCheck(device->pvkCreateImage(device->device, &imageCreateInfo, NULL, &frameBuffer->image),
		"Failed to create depth image");

	vklAllocateImageMemory(device, &frameBuffer->imageMemory, frameBuffer->image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageCreateInfo.extent.width = frameBuffer->width;
	imageCreateInfo.extent.height = frameBuffer->height;
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

	VLKCheck(device->pvkCreateImage(device->device, &imageCreateInfo, NULL, &frameBuffer->depthImage),
		"Failed to create depth image");

	vklAllocateImageMemory(device, &frameBuffer->depthImageMemory, frameBuffer->depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	device->pvkBeginCommandBuffer(devCon->setupCmdBuffer, &beginInfo);

	VkImageMemoryBarrier layoutTransitionBarrier;
	memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = 0;
	layoutTransitionBarrier.dstAccessMask = accessMask;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	layoutTransitionBarrier.newLayout = layout;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = frameBuffer->image;
	VkImageSubresourceRange resourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	layoutTransitionBarrier.subresourceRange = resourceRange;

	device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &layoutTransitionBarrier);

	memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = 0;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = frameBuffer->depthImage;
	resourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	layoutTransitionBarrier.subresourceRange = resourceRange;

	device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &layoutTransitionBarrier);

	device->pvkEndCommandBuffer(devCon->setupCmdBuffer);

	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &devCon->setupCmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	VLKCheck(device->pvkQueueSubmit(devCon->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit Queue");

	device->pvkQueueWaitIdle(devCon->queue);
	device->pvkResetCommandBuffer(devCon->setupCmdBuffer, 0);

	VkImageViewCreateInfo imageViewCreateInfo;
	memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = frameBuffer->image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = imageFormat;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VLKCheck(device->pvkCreateImageView(device->device, &imageViewCreateInfo, device->instance->allocator, &frameBuffer->imageView),
		"Failed to create image view");

	memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = frameBuffer->depthImage;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VLKCheck(device->pvkCreateImageView(device->device, &imageViewCreateInfo, device->instance->allocator, &frameBuffer->depthImageView),
		"Failed to create image view");

	VkAttachmentDescription passAttachments[2];
	memset(passAttachments, 0, sizeof(VkAttachmentDescription) * 2);
	passAttachments[0].format = imageFormat;
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

	VLKCheck(device->pvkCreateRenderPass(device->device, &renderPassCreateInfo, device->instance->allocator, &frameBuffer->renderPass),
		"Failed to create renderpass");

	VkImageView frameBufferAttachments[2];
	frameBufferAttachments[0] = frameBuffer->imageView;
	frameBufferAttachments[1] = frameBuffer->depthImageView;

	VkFramebufferCreateInfo frameBufferCreateInfo;
	memset(&frameBufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.renderPass = frameBuffer->renderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = frameBufferAttachments;
	frameBufferCreateInfo.width = frameBuffer->width;
	frameBufferCreateInfo.height = frameBuffer->height;
	frameBufferCreateInfo.layers = 1;

	VLKCheck(device->pvkCreateFramebuffer(device->device, &frameBufferCreateInfo, device->instance->allocator, &frameBuffer->frameBuffer),
		"Failed to create framebuffer");
	
	VkSamplerCreateInfo samplerCreateInfo;
	memset(&samplerCreateInfo, 0, sizeof(VkSamplerCreateInfo));
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 0;
	samplerCreateInfo.minLod = 0;
	samplerCreateInfo.maxLod = 0;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VLKCheck(device->pvkCreateSampler(device->device, &samplerCreateInfo, NULL, &frameBuffer->sampler),
		"Failed to create sampler");

	*pFrameBuffer = frameBuffer;

	return 0;
}

int vklDestroyFrameBuffer(VKLDevice* device, VKLFrameBuffer* frameBuffer) {

	device->pvkDestroyFramebuffer(device->device, frameBuffer->frameBuffer, device->instance->allocator);
	device->pvkDestroyRenderPass(device->device, frameBuffer->renderPass, device->instance->allocator);
	
	device->pvkDestroyImageView(device->device, frameBuffer->imageView, device->instance->allocator);
	device->pvkDestroyImageView(device->device, frameBuffer->depthImageView, device->instance->allocator);

	device->pvkFreeMemory(device->device, frameBuffer->imageMemory, device->instance->allocator);
	device->pvkFreeMemory(device->device, frameBuffer->depthImageMemory, device->instance->allocator);

	device->pvkDestroyImage(device->device, frameBuffer->image, device->instance->allocator);
	device->pvkDestroyImage(device->device, frameBuffer->depthImage, device->instance->allocator);

	free_c(frameBuffer);
	return 0;
}

int vklSetClearColor(VKLFrameBuffer* frameBuffer, float r, float g, float b, float a) {
	frameBuffer->clearR = r;
	frameBuffer->clearG = g;
	frameBuffer->clearB = b;
	frameBuffer->clearA = a;
	return 0;
}

int vklBeginRender(VKLDevice* device, VKLFrameBuffer* frameBuffer, VkCommandBuffer cmdBuffer) {
	VkImageMemoryBarrier layoutTransitionBarrier;
	memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = frameBuffer->accessMask;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	layoutTransitionBarrier.oldLayout = frameBuffer->layout;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = frameBuffer->image;
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
		{ frameBuffer->clearR, frameBuffer->clearG, frameBuffer->clearB, frameBuffer->clearA },
		{ 1.0, 0.0 } };

	VkRenderPassBeginInfo renderPassBeginInfo;
	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = frameBuffer->renderPass;
	renderPassBeginInfo.framebuffer = frameBuffer->frameBuffer;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = frameBuffer->width;
	renderPassBeginInfo.renderArea.extent.height = frameBuffer->height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValue;
	device->pvkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	return 0;
}

int vklEndRender(VKLDevice* device, VKLFrameBuffer* frameBuffer, VkCommandBuffer cmdBuffer) {
	device->pvkCmdEndRenderPass(cmdBuffer);

	VkImageMemoryBarrier prePresentBarrier;
	memset(&prePresentBarrier, 0, sizeof(VkImageMemoryBarrier));
	prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	prePresentBarrier.dstAccessMask = frameBuffer->accessMask;
	prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	prePresentBarrier.newLayout = frameBuffer->layout;
	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	prePresentBarrier.subresourceRange.baseMipLevel = 0;
	prePresentBarrier.subresourceRange.levelCount = 1;
	prePresentBarrier.subresourceRange.baseArrayLayer = 0;
	prePresentBarrier.subresourceRange.layerCount = 1;
	prePresentBarrier.image = frameBuffer->image;

	device->pvkCmdPipelineBarrier(cmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &prePresentBarrier);

	return 0;
}
