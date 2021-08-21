#include <VKL/VKL.h>

VKLCommandBuffer::VKLCommandBuffer(const VKLQueue* queue) {
	m_queue = queue;
	m_device = queue->device();
	
	VkCommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = NULL;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queue->getFamilyIndex();
	
	VK_CALL(m_device->vk.CreateCommandPool(m_device->handle(), &commandPoolCreateInfo, m_device->allocationCallbacks(), &m_pool));
	
	VkCommandBufferAllocateInfo commandBufferAllocationInfo;
	commandBufferAllocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocationInfo.pNext = NULL;
	commandBufferAllocationInfo.commandPool = m_pool;
	commandBufferAllocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocationInfo.commandBufferCount = 1;

	VK_CALL(m_device->vk.AllocateCommandBuffers(m_device->handle(), &commandBufferAllocationInfo, &m_handle));
}

VkCommandPool VKLCommandBuffer::pool() {
	return m_pool;
}


void VKLCommandBuffer::begin() {
	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = NULL;

	m_device->vk.BeginCommandBuffer(m_handle, &beginInfo);
}

void VKLCommandBuffer::end() {
	m_device->vk.EndCommandBuffer(m_handle);
}

void VKLCommandBuffer::reset() {
	
}

void VKLCommandBuffer::bufferBarrier(VKLBuffer* buffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
	m_device->vk.CmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, 0, 0, NULL, 1, buffer->getMemoryBarrier(), 0, NULL);
}

void VKLCommandBuffer::copyBuffer(VKLBuffer* dst, VKLBuffer* src, VkBufferCopy bufferCopy) {
	//src->setNewAccessMask(VK_ACCESS_TRANSFER_READ_BIT);
	//bufferBarrier(src, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
	//src->resetBarrier();
	
	//dst->setNewAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
	//bufferBarrier(dst, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
	//dst->resetBarrier();
	
	m_device->vk.CmdCopyBuffer(m_handle, src->handle(), dst->handle(), 1, &bufferCopy);
	
	//src->setNewAccessMask(VK_ACCESS_TRANSFER_READ_BIT);
	//bufferBarrier(src, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
	//src->resetBarrier();
	
	//dst->setNewAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
	//bufferBarrier(dst, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
	//dst->resetBarrier();
}

void VKLCommandBuffer::destroy() {
	m_device->vk.FreeCommandBuffers(m_device->handle(), m_pool, 1, &m_handle);
	m_device->vk.DestroyCommandPool(m_device->handle(), m_pool, m_device->allocationCallbacks());
}
