#include <VKL/VKL.h>

int vklCreateBuffer(VKLDevice* device, VKLBuffer** pBuffer, VkBool32 deviceLocal, size_t size, VkBufferUsageFlags usage) {
	VKLBuffer* buffer = malloc_c(sizeof(VKLBuffer));

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
	buffer->deviceLocal = deviceLocal;

	*pBuffer = buffer;

	return 0;
}

int vklDestroyBuffer(VKLDevice* device, VKLBuffer* buffer) {
	device->pvkFreeMemory(device->device, buffer->memory, device->instance->allocator);
	device->pvkDestroyBuffer(device->device, buffer->buffer, device->instance->allocator);

	free_c(buffer);

	return 0;
}

int vklCreateStagedBuffer(VKLDeviceGraphicsContext* context, VKLBuffer** pBuffer, void* data, size_t size, VkBufferUsageFlags usage) {
	VKLDevice* device = context->device;

	VKLBuffer* stagedBuffer;
	vklCreateBuffer(device, &stagedBuffer, VK_FALSE, size, usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	vklWriteToMemory(device, stagedBuffer->memory, data, size, 0);

	vklCreateBuffer(device, pBuffer, VK_TRUE, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	VKLBuffer* buffer = *pBuffer;

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

	device->pvkCmdCopyBuffer(context->setupCmdBuffer, stagedBuffer->buffer, buffer->buffer, 1, &bufferCopy);

	device->pvkEndCommandBuffer(context->setupCmdBuffer);

	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context->setupCmdBuffer;
	VLKCheck(device->pvkQueueSubmit(context->queue, 1, &submitInfo, VK_NULL_HANDLE),
		"Could not submit Queue");

	device->pvkQueueWaitIdle(context->queue);

	vklDestroyBuffer(device, stagedBuffer);

	return 0;
}
