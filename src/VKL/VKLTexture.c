#include <VKL/VKL.h>

int vklCreateTexture(VKLDevice* device, VKLTexture** pTexture, VKLTextureCreateInfo* createInfo, VkBool32 deviceLocal) {
	VKLTexture* texture = malloc_c(sizeof(VKLTexture));

	texture->width = createInfo->width;
	texture->height = createInfo->height;
	texture->colorCount = createInfo->colorCount;
	texture->colorSize = createInfo->colorSize;

	VkImageCreateInfo textureCreateInfo;
	memset(&textureCreateInfo, 0, sizeof(VkImageCreateInfo));
	textureCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	textureCreateInfo.pNext = NULL;
	textureCreateInfo.flags = 0;
	textureCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	textureCreateInfo.format = createInfo->format;
	textureCreateInfo.extent.width = createInfo->width;
	textureCreateInfo.extent.height = createInfo->height;
	textureCreateInfo.extent.depth = 1;
	textureCreateInfo.mipLevels = 1;
	textureCreateInfo.arrayLayers = 1;
	textureCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	textureCreateInfo.tiling = createInfo->tiling;
	textureCreateInfo.usage = createInfo->usage;
	textureCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	textureCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	VLKCheck(device->pvkCreateImage(device->device, &textureCreateInfo, NULL, &texture->image),
		"Failed to create texture image");

	vklAllocateImageMemory(device, &texture->memory, texture->image,
		deviceLocal == VK_TRUE ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	texture->temporary = VK_TRUE;

	if (createInfo->usage != VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
		texture->temporary = VK_FALSE;

		VkImageViewCreateInfo textureImageViewCreateInfo;
		memset(&textureImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
		textureImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		textureImageViewCreateInfo.image = texture->image;
		textureImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		textureImageViewCreateInfo.format = textureCreateInfo.format;
		textureImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		textureImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		textureImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		textureImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		textureImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		textureImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		textureImageViewCreateInfo.subresourceRange.levelCount = 1;
		textureImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		textureImageViewCreateInfo.subresourceRange.layerCount = 1;

		VLKCheck(device->pvkCreateImageView(device->device, &textureImageViewCreateInfo, NULL, &texture->imageView),
			"Failed to create image view");

		VkSamplerCreateInfo samplerCreateInfo;
		memset(&samplerCreateInfo, 0, sizeof(VkSamplerCreateInfo));
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = createInfo->filter;
		samplerCreateInfo.minFilter = createInfo->filter;
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

		VLKCheck(device->pvkCreateSampler(device->device, &samplerCreateInfo, NULL, &texture->sampler),
			"Failed to create sampler");
	}

	*pTexture = texture;

	return 0;
}

int vklSetTextureData(VKLDevice* device, VKLTexture* texture, uint8_t* data) {
	void *imageMapped;

	VLKCheck(device->pvkMapMemory(device->device, texture->memory, 0, VK_WHOLE_SIZE, 0, &imageMapped),
		"Failed to map image memory.");

	VkImageSubresource subresource;
	memset(&subresource, 0, sizeof(VkImageSubresource));
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.arrayLayer = 0;

	VkSubresourceLayout imageLayout;
	device->pvkGetImageSubresourceLayout(device->device, texture->image, &subresource, &imageLayout);

	uint32_t textureRowWidth = texture->width * texture->colorCount * texture->colorSize;

	if (imageLayout.rowPitch == textureRowWidth) {
		memcpy(imageMapped, data, texture->height * textureRowWidth);
	}
	else {
		uint8_t* dataBytes = (uint8_t*)imageMapped;

		for (int y = 0; y < texture->height; y++) {
			memcpy(
				&dataBytes[y * imageLayout.rowPitch],
				&data[y * textureRowWidth],
				textureRowWidth);
		}
	}

	VkMappedMemoryRange memoryRange;
	memset(&memoryRange, 0, sizeof(VkMappedMemoryRange));
	memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memoryRange.memory = texture->memory;
	memoryRange.offset = 0;
	memoryRange.size = VK_WHOLE_SIZE;
	device->pvkFlushMappedMemoryRanges(device->device, 1, &memoryRange);

	device->pvkUnmapMemory(device->device, texture->memory);
	
	return 0;
}

int vklCreateStagedTexture(VKLDeviceGraphicsContext* devCon, VKLTexture** pTexture, VKLTextureCreateInfo* createInfo, uint8_t* data) {
	VKLDevice* device = devCon->device;
	
	VKLTexture* stagedTexture;

	VKLTextureCreateInfo* stagedCreateInfo = malloc_c(sizeof(VKLTextureCreateInfo));
	memcpy(stagedCreateInfo, createInfo, sizeof(VKLTextureCreateInfo));
	stagedCreateInfo->usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	vklCreateTexture(device, &stagedTexture, stagedCreateInfo, VK_FALSE);

	vklSetTextureData(device, stagedTexture, data);

	VKLTexture* texture;

	VKLTextureCreateInfo* destCreateInfo = malloc_c(sizeof(VKLTextureCreateInfo));
	memcpy(destCreateInfo, createInfo, sizeof(VKLTextureCreateInfo));
	destCreateInfo->usage = destCreateInfo->usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	vklCreateTexture(device, &texture, destCreateInfo, VK_TRUE);

	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	device->pvkBeginCommandBuffer(devCon->setupCmdBuffer, &beginInfo);

	VkImageMemoryBarrier layoutTransitionBarrier;
	memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = stagedTexture->image;
	VkImageSubresourceRange resourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	layoutTransitionBarrier.subresourceRange = resourceRange;

	device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &layoutTransitionBarrier);

	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = texture->image;
	layoutTransitionBarrier.subresourceRange = resourceRange;

	device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
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
	region.extent.width = texture->width;
	region.extent.height = texture->height;
	region.extent.depth = 1;

	device->pvkCmdCopyImage(
		devCon->setupCmdBuffer,
		stagedTexture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region);

	layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	layoutTransitionBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	layoutTransitionBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransitionBarrier.image = texture->image;
	layoutTransitionBarrier.subresourceRange = resourceRange;

	device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &layoutTransitionBarrier);

	device->pvkEndCommandBuffer(devCon->setupCmdBuffer);

	VkPipelineStageFlags waitStageMash[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = waitStageMash;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &devCon->setupCmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	VLKCheck(device->pvkQueueSubmit(devCon->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit Queue");

	device->pvkQueueWaitIdle(devCon->queue);
	device->pvkResetCommandBuffer(devCon->setupCmdBuffer, 0);

	vklDestroyTexture(device, stagedTexture);

	*pTexture = texture;

	return 0;
}

int vklDestroyTexture(VKLDevice* device, VKLTexture* texture) {
	if (texture->temporary == VK_FALSE) {
		device->pvkDestroySampler(device->device, texture->sampler, device->instance->allocator);
		device->pvkDestroyImageView(device->device, texture->imageView, device->instance->allocator);
	}

	device->pvkFreeMemory(device->device, texture->memory, device->instance->allocator);
	device->pvkDestroyImage(device->device, texture->image, device->instance->allocator);

	free_c(texture);

	return 0;
}

