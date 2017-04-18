#include <VKL/VKL.h>

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

int vklAllocateMemory(VKLDevice* device, VkDeviceMemory* memory, VkMemoryPropertyFlags desiredMemoryFlags, VkMemoryRequirements memoryRequirements) {
	VkMemoryAllocateInfo imageAllocateInfo;
	memset(&imageAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	imageAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageAllocateInfo.allocationSize = memoryRequirements.size;

	uint32_t memoryTypeBits = memoryRequirements.memoryTypeBits;
	for (uint32_t i = 0; i < 32; ++i) {
		VkMemoryType memoryType = device->memoryProperties.memoryTypes[i];
		if (memoryTypeBits & 1) {
			if ((memoryType.propertyFlags & desiredMemoryFlags) == desiredMemoryFlags) {
				imageAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		memoryTypeBits = memoryTypeBits >> 1;
	}

	VLKCheck(device->pvkAllocateMemory(device->device, &imageAllocateInfo, device->instance->allocator, memory),
		"Failed to allocate device memory");

	return 0;
}

int vklAllocateImageMemory(VKLDevice* device, VkDeviceMemory* memory, VkImage image, VkMemoryPropertyFlags desiredMemoryFlags) {
	VkMemoryRequirements memoryRequirements;
	memset(&memoryRequirements, 0, sizeof(VkMemoryRequirements));
	device->pvkGetImageMemoryRequirements(device->device, image, &memoryRequirements);

	vklAllocateMemory(device, memory, desiredMemoryFlags, memoryRequirements);

	VLKCheck(device->pvkBindImageMemory(device->device, image, *memory, 0),
		"Failed to bind image memory");

	return 0;
}

int vklAllocateBufferMemory(VKLDevice* device, VkDeviceMemory* memory, VkBuffer buffer, VkMemoryPropertyFlags desiredMemoryFlags) {
	VkMemoryRequirements memoryRequirements;
	memset(&memoryRequirements, 0, sizeof(VkMemoryRequirements));
	device->pvkGetBufferMemoryRequirements(device->device, buffer, &memoryRequirements);

	vklAllocateMemory(device, memory, desiredMemoryFlags, memoryRequirements);

	VLKCheck(device->pvkBindBufferMemory(device->device, buffer, *memory, 0),
		"Failed to bind image memory");

	return 0;
}

int vklWriteToMemory(VKLDevice* device, VkDeviceMemory memory, void* data, size_t size) {
	void *mapped;
	VLKCheck(device->pvkMapMemory(device->device, memory, 0, VK_WHOLE_SIZE, 0, &mapped),
		"Failed to map buffer memory");

	memcpy(mapped, data, size);

	device->pvkUnmapMemory(device->device, memory);
	return 0;
}

int vklCreateBuffer(VKLDevice* device, VKLBuffer* buffer, VkBool32 deviceLocal, size_t size, VkBufferUsageFlags usage) {
	VkBufferCreateInfo bufferCreateInfo;
	memset(&bufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VLKCheck(device->pvkCreateBuffer(device->device, &bufferCreateInfo, device->instance->allocator, &buffer->buffer),
		"Failed to create vertex input buffer.");

	VkMemoryPropertyFlags memoryPropertieFlag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	if (deviceLocal) {
		memoryPropertieFlag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}

	vklAllocateBufferMemory(device, &buffer->memory, buffer->buffer, memoryPropertieFlag);

	buffer->size = size;

	return 0;
}

int vklDestroyBuffer(VKLDevice* device, VKLBuffer* buffer) {
	device->pvkFreeMemory(device->device, buffer->memory, device->instance->allocator);
	device->pvkDestroyBuffer(device->device, buffer->buffer, device->instance->allocator);
}

int vklCreateStagedBuffer(VKLDeviceGraphicsContext* context, VKLBuffer* buffer, void* data, size_t size, VkBufferUsageFlags usage) {
	VKLDevice* device = context->device;

	VKLBuffer stagedBuffer;
	vklCreateBuffer(device, &stagedBuffer, 0, size, usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	vklWriteToMemory(device, stagedBuffer.memory, data, size);

	vklCreateBuffer(device, buffer, 1, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	device->pvkBeginCommandBuffer(context->setupCmdBuffer, &beginInfo);

	VkBufferCopy bufferCopy;
	memset(&bufferCopy, 0, sizeof(VkBufferCopy));
	bufferCopy.dstOffset = 0;
	bufferCopy.srcOffset = 0;
	bufferCopy.size = size;

	device->pvkCmdCopyBuffer(context->setupCmdBuffer, stagedBuffer.buffer, buffer->buffer, 1, &bufferCopy);

	device->pvkEndCommandBuffer(context->setupCmdBuffer);

	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context->setupCmdBuffer;
	VLKCheck(device->pvkQueueSubmit(context->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit Queue");

	device->pvkQueueWaitIdle(context->queue);

	vklDestroyBuffer(device, &stagedBuffer);

	return 0;
}