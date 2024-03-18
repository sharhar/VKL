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
	VK_CALL(m_device->vk.MapMemory(m_device->handle(), m_allocation.memory, m_allocation.offset, m_allocation.size, 0, (void**)&mappedData));	
	memcpy(mappedData + offset, data, size);
	m_device->vk.UnmapMemory(m_device->handle(), m_allocation.memory);
}

void VKLBuffer::setData(const VKLQueue* transferQueue, VKLBuffer* stagingBuffer, void* data, size_t size, size_t offset) {
	LOG_INFO("Setting data through staging buffer");
	stagingBuffer->setData(data, size, 0);
	
	VkBufferCopy bufferCopy;
	bufferCopy.size = size;
	bufferCopy.dstOffset = offset;
	bufferCopy.srcOffset = 0;
	
	LOG_INFO("Copying data from staging buffer to buffer");

	copyFrom(stagingBuffer, transferQueue, bufferCopy);
}

void* VKLBuffer::map() {
	void* result;
	VK_CALL(m_device->vk.MapMemory(m_device->handle(), m_allocation.memory, m_allocation.offset, m_allocation.size, 0, &result));
	return result;
}
void VKLBuffer::unmap() {
	m_device->vk.UnmapMemory(m_device->handle(), m_allocation.memory);
}

void VKLBuffer::getData(void* data, size_t size, size_t offset) {
	uint8_t* mappedData;
	VK_CALL(m_device->vk.MapMemory(m_device->handle(), m_allocation.memory, m_allocation.offset, m_allocation.size, 0, (void**)&mappedData));
	memcpy(data, mappedData + offset, size);
	m_device->vk.UnmapMemory(m_device->handle(), m_allocation.memory);
}

VKLAllocation VKLBuffer::allocation() const {
	return m_allocation;
}

VkMemoryRequirements VKLBuffer::memoryRequirements() const {
	VkMemoryRequirements memoryRequirements;
	m_device->vk.GetBufferMemoryRequirements(m_device->handle(), m_handle, &memoryRequirements);
	return memoryRequirements;
}

void VKLBuffer::getData(const VKLQueue* transferQueue, VKLBuffer* stagingBuffer, void* data, size_t size, size_t offset) {
	VkBufferCopy bufferCopy;
	bufferCopy.size = size;
	bufferCopy.dstOffset = 0;
	bufferCopy.srcOffset = offset;
	
	stagingBuffer->copyFrom(this, transferQueue, bufferCopy);
	
	stagingBuffer->getData(data, size, 0);
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
	
	transferQueue->submitAndWait(transferQueue->getCmdBuffer());
	
	transferQueue->getCmdBuffer()->reset();
}

void VKLBuffer::uploadData(const VKLQueue* transferQueue, void* data, size_t size, size_t offset) {
	VKLBufferCreateInfo tempBufferCreateInfo;
	
	LOG_INFO("Creating temp buffer for upload");

	tempBufferCreateInfo.device(m_device).size(size)
						.memoryProperties(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
						.usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	

	VKLBuffer tempStageBuffer(tempBufferCreateInfo);

	VKLAllocation allocation;
	allocation.memory = m_device->allocateMemory(tempStageBuffer.memoryRequirements(), 
													VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	allocation.offset = 0;
	allocation.size = tempStageBuffer.memoryRequirements().size;

	tempStageBuffer.bind(allocation);
	
	LOG_INFO("Uploading data through temp buffer");

	setData(transferQueue, &tempStageBuffer, data, size, offset);

	LOG_INFO("Destroying temp buffer...");

	tempStageBuffer.destroy();

	m_device->vk.FreeMemory(m_device->handle(), allocation.memory, m_device->allocationCallbacks());
}

void VKLBuffer::downloadData(const VKLQueue* transferQueue, void* data, size_t size, size_t offset) {
	VKLBufferCreateInfo tempBufferCreateInfo;
	tempBufferCreateInfo.device(m_device).size(size)
						.memoryProperties(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
						.usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	
	VKLBuffer tempStageBuffer(tempBufferCreateInfo);

	VKLAllocation allocation;
	allocation.memory = m_device->allocateMemory(tempStageBuffer.memoryRequirements(), 
													VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	allocation.offset = 0;
	allocation.size = tempStageBuffer.memoryRequirements().size;
	
	tempStageBuffer.bind(allocation);
	
	getData(transferQueue, &tempStageBuffer, data, size, offset);
	
	tempStageBuffer.destroy();

	m_device->vk.FreeMemory(m_device->handle(), allocation.memory, m_device->allocationCallbacks());
}

void VKLBuffer::_create(const VKLBufferCreateInfo& createInfo) {
	m_device = createInfo.m_device;
	
	VK_CALL(m_device->vk.CreateBuffer(m_device->handle(), &createInfo.m_bufferCreateInfo, m_device->allocationCallbacks(), &m_handle));
	
	m_memoryBarrier.buffer = m_handle;
	m_memoryBarrier.size = createInfo.m_bufferCreateInfo.size;
}

void VKLBuffer::bind(VKLAllocation allocation) {
	LOG_INFO("Binding buffer to memory: %d, %d, %d", allocation.memory, allocation.offset, allocation.size);

	m_allocation = allocation;

	VK_CALL(m_device->vk.BindBufferMemory(m_device->handle(), m_handle, m_allocation.memory, m_allocation.offset));
}

void VKLBuffer::_destroy() {
	m_device->vk.DestroyBuffer(m_device->handle(), m_handle, m_device->allocationCallbacks());
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

	m_memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	m_allocationPNext = NULL;
}

VKLBufferCreateInfo& VKLBufferCreateInfo::pNext(void* pNext) {
	m_bufferCreateInfo.pNext = pNext;
	
	return invalidate();
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

VKLBufferCreateInfo& VKLBufferCreateInfo::allocationPNext(void* pNext) {
	m_allocationPNext = pNext;
	
	return invalidate();
}
	
VKLBufferCreateInfo& VKLBufferCreateInfo::memoryProperties(VkMemoryPropertyFlags memoryProperties) {
	m_memoryProperties = memoryProperties;

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
	
	return true;
}
