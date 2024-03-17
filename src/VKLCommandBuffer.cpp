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

VkCommandPool VKLCommandBuffer::pool() const {
	return m_pool;
}

void VKLCommandBuffer::bufferBarrier(VKLBuffer* buffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) const {
	m_device->vk.CmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, 0, 0, NULL, 1, buffer->getMemoryBarrier(), 0, NULL);
}

void VKLCommandBuffer::copyBuffer(const VKLBuffer* dst, const VKLBuffer* src, VkBufferCopy bufferCopy) const {
	m_device->vk.CmdCopyBuffer(m_handle, src->handle(), dst->handle(), 1, &bufferCopy);
}

void VKLCommandBuffer::copyImage(const VKLImage* dst, const VKLImage* src, VkImageCopy imageCopy) const {
	m_device->vk.CmdCopyImage(m_handle, src->handle(), src->layout(), dst->handle(), dst->layout(), 1, &imageCopy);
}

void VKLCommandBuffer::begin() const {
	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = NULL;

	LOG_INFO("VKLCommandBuffer::begin: %p", m_handle);
	LOG_INFO("VKLCommandBuffer::device %p", m_device);
	LOG_INFO("VKLCommandBuffer::cmdBuffer %p", &m_device->vk);
	LOG_INFO("VKLCommandBuffer::cmdBufferFunc %p", &(m_device->vk.BeginCommandBuffer));
	LOG_INFO("VKLCommandBuffer::cmdBufferFunc %p", &(m_device->vk.BeginCommandBuffer));

	m_device->vk.BeginCommandBuffer(m_handle, &beginInfo);
}

void VKLCommandBuffer::end() const {
	m_device->vk.EndCommandBuffer(m_handle);
}

void VKLCommandBuffer::reset() const {
	
}

void VKLCommandBuffer::beginRenderPass(const VKLFramebuffer& framebuffer, VkSubpassContents contents) const  {
	beginRenderPass(&framebuffer, contents);
}

void VKLCommandBuffer::beginRenderPass(const VKLFramebuffer& framebuffer, VkSubpassContents contents, VkRect2D area) const {
	beginRenderPass(&framebuffer, contents, area);
}

void VKLCommandBuffer::beginRenderPass(const VKLFramebuffer* framebuffer, VkSubpassContents contents) const {
	VkRect2D area;
	area.offset.x = 0;
	area.offset.y = 0;
	area.extent.width = framebuffer->size().width;
	area.extent.height = framebuffer->size().height;
	
	beginRenderPass(framebuffer, contents, area);
}

void VKLCommandBuffer::beginRenderPass(const VKLFramebuffer* framebuffer, VkSubpassContents contents, VkRect2D area) const {
	VkRenderPassBeginInfo renderPassBeginInfo;
	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = framebuffer->renderPass()->handle();
	renderPassBeginInfo.framebuffer = framebuffer->handle();
	renderPassBeginInfo.renderArea = area;
	renderPassBeginInfo.clearValueCount = framebuffer->attachments().size();
	renderPassBeginInfo.pClearValues = framebuffer->clearValues();

	m_device->vk.CmdBeginRenderPass(m_handle, &renderPassBeginInfo, contents);
}

void VKLCommandBuffer::nextSubpass(VkSubpassContents contents) const {
	m_device->vk.CmdNextSubpass(m_handle, contents);
}

void VKLCommandBuffer::endRenderPass() const {
	m_device->vk.CmdEndRenderPass(m_handle);
}

void VKLCommandBuffer::setViewPort(uint32_t index, uint32_t width, uint32_t height) const {
	setViewPort(index, 0, 0, width, height);
}

void VKLCommandBuffer::setViewPort(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height) const {
	setViewPort(index, x, y, width, height, 0, 1);
}

void VKLCommandBuffer::setViewPort(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t z, uint32_t depth) const {
	VkViewport viewport;
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = z;
	viewport.maxDepth = depth;
	
	m_device->vk.CmdSetViewport(m_handle, index, 1, &viewport);
}

void VKLCommandBuffer::setScissor(uint32_t index, uint32_t width, uint32_t height) const {
	setScissor(index, 0, 0, width, height);
}

void VKLCommandBuffer::setScissor(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height) const {
	VkRect2D scissor;
	scissor.offset.x = x;
	scissor.offset.y = y;
	scissor.extent.width = width;
	scissor.extent.height = height;
	
	m_device->vk.CmdSetScissor(m_handle, index, 1, &scissor);
}

void VKLCommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
	m_device->vk.CmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VKLCommandBuffer::bindVertexBuffer(const VKLBuffer& buffer, uint32_t binding, VkDeviceSize offset) const {
	bindVertexBuffer(&buffer, binding, offset);
}

void VKLCommandBuffer::bindVertexBuffer(const VKLBuffer* buffer, uint32_t binding, VkDeviceSize offset) const {
	VkBuffer buff = buffer->handle();
	m_device->vk.CmdBindVertexBuffers(m_handle, binding, 1, &buff, &offset);
}

void VKLCommandBuffer::bindPipeline(const VKLPipeline& pipeline) const {
	bindPipeline(&pipeline);
}

void VKLCommandBuffer::bindPipeline(const VKLPipeline* pipeline) const {
	m_device->vk.CmdBindPipeline(m_handle, pipeline->layout()->bindPoint(), pipeline->handle());
}

void VKLCommandBuffer::bindDescriptorSet(const VKLDescriptorSet& descriptorSet) const {
	bindDescriptorSet(&descriptorSet);
}

void VKLCommandBuffer::bindDescriptorSet(const VKLDescriptorSet* descriptorSet) const {
	VkDescriptorSet descSet = descriptorSet->handle();
	m_device->vk.CmdBindDescriptorSets(m_handle, descriptorSet->layout()->bindPoint(), descriptorSet->layout()->handle(), descriptorSet->set(), 1, &descSet, 0, NULL);
}

void VKLCommandBuffer::pushConstants(const VKLPipeline& pipeline, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) const {
	pushConstants(&pipeline, stageFlags, offset, size, pValues);
}

void VKLCommandBuffer::pushConstants(const VKLPipeline* pipeline, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) const {
	m_device->vk.CmdPushConstants(m_handle, pipeline->layout()->handle(), stageFlags, offset, size, pValues);
}

void VKLCommandBuffer::dispatch(uint32_t x, uint32_t y, uint32_t z) const {
	m_device->vk.CmdDispatch(m_handle, x, y, z);
}

void VKLCommandBuffer::destroy() {
	m_device->vk.FreeCommandBuffers(m_device->handle(), m_pool, 1, &m_handle);
	m_device->vk.DestroyCommandPool(m_device->handle(), m_pool, m_device->allocationCallbacks());
}
