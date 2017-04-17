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
}

int vklAllocateImageMemory(VKLDevice* device, VkDeviceMemory* memory, VkImage image, VkMemoryPropertyFlags desiredMemoryFlags) {
	VkMemoryRequirements memoryRequirements;
	memset(&memoryRequirements, 0, sizeof(VkMemoryRequirements));
	device->pvkGetImageMemoryRequirements(device->device, image, &memoryRequirements);

	vklAllocateMemory(device, memory, desiredMemoryFlags, memoryRequirements);

	VLKCheck(device->pvkBindImageMemory(device->device, image, *memory, 0),
		"Failed to bind image memory");
}