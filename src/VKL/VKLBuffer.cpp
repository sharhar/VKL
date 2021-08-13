#include <VKL/VKL.h>

VKLBuffer::VKLBuffer() : VKLCreator<VKLBufferCreateInfo>("VKLBuffer") {
	m_device = NULL;
	
	memset(&m_memoryBarrier, 0, sizeof(VkBufferMemoryBarrier));
	m_memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	m_memoryBarrier.pNext = NULL;
	m_memoryBarrier.srcAccessMask = 0;
	m_memoryBarrier.dstAccessMask = 0;
	m_memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.offset = 0;
}


VKLBuffer::VKLBuffer(const VKLBufferCreateInfo& createInfo) : VKLCreator<VKLBufferCreateInfo>("VKLBuffer")  {
	memset(&m_memoryBarrier, 0, sizeof(VkBufferMemoryBarrier));
	m_memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	m_memoryBarrier.pNext = NULL;
	m_memoryBarrier.srcAccessMask = 0;
	m_memoryBarrier.dstAccessMask = 0;
	m_memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.offset = 0;
	
	this->create(createInfo);
}

void VKLBuffer::setData(void* data, size_t size, size_t offset) {
	uint8_t* mappedData;
	vmaMapMemory(m_device->allocator(), m_allocation, (void**)&mappedData);
	memcpy(mappedData + offset, data, size);
	vmaUnmapMemory(m_device->allocator(), m_allocation);
}

void VKLBuffer::setNewAccessMask(VkAccessFlags accessMask) {
	m_memoryBarrier.dstAccessMask = accessMask;
}


VkBufferMemoryBarrier* VKLBuffer::getMemoryBarrier() {
	return &m_memoryBarrier;
}


void VKLBuffer::resetBarrier() {
	m_memoryBarrier.srcAccessMask = m_memoryBarrier.dstAccessMask;
}

void VKLBuffer::copyFrom(VKLBuffer* src, const VKLQueue* transferQueue, VkBufferCopy bufferCopy) {
	transferQueue->getCmdBuffer()->begin();
	transferQueue->getCmdBuffer()->copyBuffer(this, src, bufferCopy);
	transferQueue->getCmdBuffer()->end();
	
	VkFence fence = m_device->createFence(0);
	transferQueue->submit(transferQueue->getCmdBuffer(), fence);
	m_device->waitForFence(fence);
	m_device->destroyFence(fence);
	
	transferQueue->getCmdBuffer()->reset();
}

void VKLBuffer::uploadData(const VKLQueue* transferQueue, void* data, size_t size, size_t offset) {
	VKLBufferCreateInfo tempBufferCreateInfo;
	tempBufferCreateInfo.device(m_device).size(size)
						.memoryUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
						.usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	
	VKLBuffer tempStageBuffer(tempBufferCreateInfo);
	
	tempStageBuffer.setData(data, size, 0);
	
	VkBufferCopy bufferCopy;
	bufferCopy.size = size;
	bufferCopy.dstOffset = offset;
	bufferCopy.srcOffset = 0;
	
	copyFrom(&tempStageBuffer, transferQueue, bufferCopy);
	
	tempStageBuffer.destroy();
}

void VKLBuffer::_create(const VKLBufferCreateInfo& createInfo) {
	m_device = createInfo.m_device;
	
	VK_CALL(vmaCreateBuffer(m_device->allocator(), &createInfo.m_bufferCreateInfo,
							&createInfo.m_allocationCreateInfo, &m_handle, &m_allocation, NULL));
	m_memoryBarrier.buffer = m_handle;
	m_memoryBarrier.size = createInfo.m_bufferCreateInfo.size;
}

void VKLBuffer::_destroy() {
	vmaDestroyBuffer(m_device->allocator(), m_handle, m_allocation);
}

VKLBufferCreateInfo::VKLBufferCreateInfo() {
	m_device = NULL;
	
	memset(&m_bufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	m_bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	m_bufferCreateInfo.pNext = NULL;
	m_bufferCreateInfo.flags = 0;
	m_bufferCreateInfo.usage = 0;
	m_bufferCreateInfo.size = 0;
	m_bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_bufferCreateInfo.queueFamilyIndexCount = 0;
	m_bufferCreateInfo.pQueueFamilyIndices = NULL;
	
	memset(&m_allocationCreateInfo, 0, sizeof(VmaAllocationCreateInfo));
	m_allocationCreateInfo.flags = 0;
	m_allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
	m_allocationCreateInfo.memoryTypeBits = 0;
	m_allocationCreateInfo.pUserData = NULL;
	m_allocationCreateInfo.pool = VK_NULL_HANDLE;
	m_allocationCreateInfo.requiredFlags = 0;
	m_allocationCreateInfo.preferredFlags = 0;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::device(const VKLDevice* device) {
	m_device = device;
	
	return invalidate();
}

VKLBufferCreateInfo& VKLBufferCreateInfo::size(VkDeviceSize size) {
	m_bufferCreateInfo.size = size;
	
	return invalidate();
}

VKLBufferCreateInfo& VKLBufferCreateInfo::usage(VkBufferUsageFlags usage) {
	m_bufferCreateInfo.usage = usage;
	
	return invalidate();
}

VKLBufferCreateInfo& VKLBufferCreateInfo::allocationFlags(VmaAllocationCreateFlags flags) {
	m_allocationCreateInfo.flags = flags;
	
	return invalidate();
}

VKLBufferCreateInfo& VKLBufferCreateInfo::memoryUsage(VmaMemoryUsage memoryUsage) {
	m_allocationCreateInfo.usage = memoryUsage;
	
	return invalidate();
}

bool VKLBufferCreateInfo::_validate() {
	if(m_device == NULL) {
		printf("VKL Validation Error: VKLBufferCreateInfo::device is not set!\n");
		return false;
	}

	if (m_bufferCreateInfo.size == 0) {
		printf("VKL Validation Error: VKLBufferCreateInfo::size is not set!\n");
		return false;
	}

	if (m_allocationCreateInfo.flags == 0 && m_allocationCreateInfo.usage == VMA_MEMORY_USAGE_UNKNOWN) {
		printf("VKL Validation Error: No memory usage or flags are set in VKLBufferCreateInfo!\n");
		return false;
	}
	
	return true;
}
