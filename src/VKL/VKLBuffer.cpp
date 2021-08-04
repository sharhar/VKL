#include <VKL/VKL.h>

VKLBuffer::VKLBuffer() : VKLBuilder<VKLBufferCreateInfo>("VKLBuffer") {
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


VKLBuffer::VKLBuffer(const VKLBufferCreateInfo& createInfo) : VKLBuilder<VKLBufferCreateInfo>("VKLBuffer")  {
	memset(&m_memoryBarrier, 0, sizeof(VkBufferMemoryBarrier));
	m_memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	m_memoryBarrier.pNext = NULL;
	m_memoryBarrier.srcAccessMask = 0;
	m_memoryBarrier.dstAccessMask = 0;
	m_memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.offset = 0;
	
	this->build(createInfo);
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

void VKLBuffer::copyFrom(VKLBuffer* src, VKLQueue* transferQueue, VkBufferCopy bufferCopy) {
	transferQueue->getCmdBuffer()->begin();
	transferQueue->getCmdBuffer()->copyBuffer(this, src, bufferCopy);
	transferQueue->getCmdBuffer()->end();
	
	VkFence fence = m_device->createFence(0);
	transferQueue->submit(transferQueue->getCmdBuffer(), fence);
	m_device->waitForFence(fence);
	m_device->destroyFence(fence);
	
	transferQueue->getCmdBuffer()->reset();
}

void VKLBuffer::uploadData(VKLQueue* transferQueue, void* data, size_t size, size_t offset) {
	VKLBufferCreateInfo tempBufferCreateInfo;
	tempBufferCreateInfo.setDevice(m_device).setSize(size)
						.setMemoryUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
						.setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	
	VKLBuffer tempStageBuffer(tempBufferCreateInfo);
	
	tempStageBuffer.setData(data, size, 0);
	
	VkBufferCopy bufferCopy;
	bufferCopy.size = size;
	bufferCopy.dstOffset = offset;
	bufferCopy.srcOffset = 0;
	
	copyFrom(&tempStageBuffer, transferQueue, bufferCopy);
	
	tempStageBuffer.destroy();
}

void VKLBuffer::_build(const VKLBufferCreateInfo& createInfo) {
	m_device = createInfo.device;
	
	VK_CALL(vmaCreateBuffer(m_device->allocator(), &createInfo.bufferCreateInfo,
							&createInfo.allocationCreateInfo, &m_handle, &m_allocation, NULL));
	m_memoryBarrier.buffer = m_handle;
	m_memoryBarrier.size = createInfo.bufferCreateInfo.size;
}

void VKLBuffer::destroy() {
	vmaDestroyBuffer(m_device->allocator(), m_handle, m_allocation);
}

VKLBufferCreateInfo::VKLBufferCreateInfo() {
	device = NULL;
	
	memset(&bufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = NULL;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.usage = 0;
	bufferCreateInfo.size = 0;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = NULL;
	
	memset(&allocationCreateInfo, 0, sizeof(VmaAllocationCreateInfo));
	allocationCreateInfo.flags = 0;
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
	allocationCreateInfo.memoryTypeBits = 0;
	allocationCreateInfo.pUserData = NULL;
	allocationCreateInfo.pool = VK_NULL_HANDLE;
	allocationCreateInfo.requiredFlags = 0;
	allocationCreateInfo.preferredFlags = 0;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::setDevice(const VKLDevice* device) {
	this->device = device;
	
	return *this;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::setSize(VkDeviceSize size) {
	bufferCreateInfo.size = size;
	
	return *this;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::setUsage(VkBufferUsageFlags usage) {
	bufferCreateInfo.usage = usage;
	
	return *this;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::setAllocationFlags(VmaAllocationCreateFlags flags) {
	allocationCreateInfo.flags = flags;
	
	return *this;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::setMemoryUsage(VmaMemoryUsage memoryUsage) {
	allocationCreateInfo.usage = memoryUsage;
	
	return *this;
}

bool VKLBufferCreateInfo::validate() {
	if(device == NULL || bufferCreateInfo.size == 0) {
		return false;
	}
	
	return true;
}
