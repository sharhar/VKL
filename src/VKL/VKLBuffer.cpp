#include <VKL/VKL.h>

VKLBuffer::VKLBuffer() {
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

void VKLBuffer::setData(void* data, size_t size, size_t offset) {
	uint8_t* mappedData;
	vmaMapMemory(m_device->allocator(), m_allocation, (void**)&mappedData);
	memcpy(mappedData + offset, data, size);
	vmaUnmapMemory(m_device->allocator(), m_allocation);
}

VKLBuffer& VKLBuffer::setDevice(VKLDevice* device) {
	m_device = device;
	
	return *this;
}

VKLBuffer& VKLBuffer::ciSetSize(VkDeviceSize size) {
	m_bufferCreateInfo.size = size;
	
	return *this;
}

VKLBuffer& VKLBuffer::ciAddUsage(VkBufferUsageFlags usage) {
	m_bufferCreateInfo.usage = m_bufferCreateInfo.usage | usage;
	
	return *this;
}

VKLBuffer& VKLBuffer::ciAddAllocationFlag(VmaAllocationCreateFlags flags) {
	m_allocationCreateInfo.flags = m_allocationCreateInfo.flags | flags;
	
	return *this;
}

VKLBuffer& VKLBuffer::ciSetMemoryUsage(VmaMemoryUsage memoryUsage) {
	m_allocationCreateInfo.usage = memoryUsage;
	
	return *this;
}

void VKLBuffer::_build() {
	VK_CALL(vmaCreateBuffer(m_device->allocator(), &m_bufferCreateInfo, &m_allocationCreateInfo, &m_handle, &m_allocation, NULL));
}

void VKLBuffer::destroy() {
	vmaDestroyBuffer(m_device->allocator(), m_handle, m_allocation);
}

