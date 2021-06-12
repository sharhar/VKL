#include <VKL/VKL.h>

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

int vklWriteToMemory(VKLDevice* device, VkDeviceMemory memory, void* data, size_t size, size_t offset) {
	void *mapped;
	VLKCheck(device->pvkMapMemory(device->device, memory, 0, VK_WHOLE_SIZE, 0, &mapped),
		"Failed to map buffer memory");

	memcpy(mapped+offset, data, size);

	device->pvkUnmapMemory(device->device, memory);
	return 0;
}

int vklReadFromMemory(VKLDevice* device, VkDeviceMemory memory, void* data, size_t size, size_t offset) {
	void *mapped;
	VLKCheck(device->pvkMapMemory(device->device, memory, 0, VK_WHOLE_SIZE, 0, &mapped),
		"Failed to map buffer memory");

	memcpy(data, mapped+offset, size);

	device->pvkUnmapMemory(device->device, memory);
	return 0;
}
