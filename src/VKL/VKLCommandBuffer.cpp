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

void VKLCommandBuffer::nextSubpass(VkSubpassContents contents) {
	m_device->vk.CmdNextSubpass(m_handle, contents);
}

void VKLCommandBuffer::endRenderPass() {
	m_device->vk.CmdEndRenderPass(m_handle);
}

void VKLCommandBuffer::setViewPort(uint32_t index, uint32_t width, uint32_t height) {
	setViewPort(index, 0, 0, width, height);
}

void VKLCommandBuffer::setViewPort(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	setViewPort(index, x, y, width, height, 0, 1);
}

void VKLCommandBuffer::setViewPort(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t z, uint32_t depth) {
	VkViewport viewport;
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = z;
	viewport.maxDepth = depth;
	
	m_device->vk.CmdSetViewport(m_handle, index, 1, &viewport);
}

void VKLCommandBuffer::setScissor(uint32_t index, uint32_t width, uint32_t height) {
	setScissor(index, 0, 0, width, height);
}

void VKLCommandBuffer::setScissor(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	VkRect2D scissor;
	scissor.offset.x = x;
	scissor.offset.y = y;
	scissor.extent.width = width;
	scissor.extent.height = height;
	
	m_device->vk.CmdSetScissor(m_handle, index, 1, &scissor);
}

void VKLCommandBuffer::bufferBarrier(VKLBuffer* buffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
	m_device->vk.CmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, 0, 0, NULL, 1, buffer->getMemoryBarrier(), 0, NULL);
}

void VKLCommandBuffer::copyBuffer(const VKLBuffer* dst, const VKLBuffer* src, VkBufferCopy bufferCopy) {
	m_device->vk.CmdCopyBuffer(m_handle, src->handle(), dst->handle(), 1, &bufferCopy);
}

void VKLCommandBuffer::copyImage(const VKLImage* dst, const VKLImage* src, VkImageCopy imageCopy) {
	m_device->vk.CmdCopyImage(m_handle, src->handle(), src->layout(), dst->handle(), dst->layout(), 1, &imageCopy);
}

void VKLCommandBuffer::destroy() {
	m_device->vk.FreeCommandBuffers(m_device->handle(), m_pool, 1, &m_handle);
	m_device->vk.DestroyCommandPool(m_device->handle(), m_pool, m_device->allocationCallbacks());
}
