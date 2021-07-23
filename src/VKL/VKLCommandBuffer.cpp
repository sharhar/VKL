#include <VKL/VKL.h>

VKLCommandBuffer::VKLCommandBuffer(VKLQueue* queue) {
	m_queue = queue;
	m_device = queue->getDevice();
	
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

VkCommandBuffer VKLCommandBuffer::handle() {
	return m_handle;
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

void VKLCommandBuffer::imageBarrier(VKLImage* image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
	m_device->vk.CmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, image->getMemoryBarrier());
}

void VKLCommandBuffer::destroy() {
	m_device->vk.FreeCommandBuffers(m_device->handle(), m_pool, 1, &m_handle);
	m_device->vk.DestroyCommandPool(m_device->handle(), m_pool, m_device->allocationCallbacks());
}

void VKLCommandBuffer::beginRender(VKLRenderTarget* renderTarget) {
	renderTarget->preRenderCallback(this);
	
	VkRenderPassBeginInfo renderPassBeginInfo;
	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderTarget->getRenderPass();
	renderPassBeginInfo.framebuffer = renderTarget->getCurrentFramebuffer();
	renderPassBeginInfo.renderArea = renderTarget->getRenderArea();
	renderPassBeginInfo.clearValueCount = renderTarget->getClearValueCount();
	renderPassBeginInfo.pClearValues = renderTarget->getClearValues();
	
	m_device->vk.CmdBeginRenderPass(m_handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VKLCommandBuffer::endRender(VKLRenderTarget* renderTarget) {
	m_device->vk.CmdEndRenderPass(m_handle);
	
	renderTarget->postRenderCallback(this);
}
