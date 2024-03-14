#include <VKL/VKL.h>

VKLQueue::VKLQueue() {
	m_handle = (VkQueue)VK_NULL_HANDLE;
	m_device = NULL;
	m_familyIndex = -1;
}

void VKLQueue::init(const VKLDevice* device, VkQueue queue, uint32_t familyIndex) {
	m_handle = queue;
	m_device = device;
	m_familyIndex = familyIndex;
	
	m_fence = m_device->createFence(0);
	
	m_cmdBuffer = new VKLCommandBuffer(this);
}

const VKLDevice* VKLQueue::device() const {
	return m_device;
}

VKLCommandBuffer* VKLQueue::getCmdBuffer() const {
	return m_cmdBuffer;
}

uint32_t VKLQueue::getFamilyIndex() const {
	return m_familyIndex;
}

void VKLQueue::submitAndWait(const VKLCommandBuffer* cmdBuffer) const {
	submitAndWait(cmdBuffer, 0, NULL, NULL);
}

void VKLQueue::submitAndWait(const VKLCommandBuffer* cmdBuffer, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores, const VkPipelineStageFlags* pWaitDstStageMask) const {
	submit(cmdBuffer, m_fence, NULL, waitSemaphoreCount, pWaitSemaphores, pWaitDstStageMask);
	m_device->waitForFence(m_fence);
	m_device->resetFence(m_fence);
}

void VKLQueue::submit(const VKLCommandBuffer* cmdBuffer, VkFence fence) const {
	submit(cmdBuffer, fence, NULL, 0, NULL, NULL);
}

void VKLQueue::submit(const VKLCommandBuffer* cmdBuffer, VkFence fence, const VkSemaphore* signalSempahore) const {
	submit(cmdBuffer, fence, signalSempahore, 0, NULL, NULL);
}

void VKLQueue::submit(const VKLCommandBuffer* cmdBuffer, VkFence fence, const VkSemaphore* signalSempahore, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores, const VkPipelineStageFlags* pWaitDstStageMask) const {
	VkCommandBuffer cmdBuffHandle = cmdBuffer->handle();
	
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = waitSemaphoreCount;
	submitInfo.pWaitSemaphores = pWaitSemaphores;
	submitInfo.pWaitDstStageMask = pWaitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffHandle;
	submitInfo.signalSemaphoreCount = signalSempahore == NULL ? 0 : 1;
	submitInfo.pSignalSemaphores = signalSempahore;
	
	VK_CALL(m_device->vk.QueueSubmit(m_handle, 1, &submitInfo, fence));
}

void VKLQueue::waitIdle() const {
	m_device->vk.QueueWaitIdle(m_handle);
}


void VKLQueue::destroy() {
	m_cmdBuffer->destroy();
	m_device->destroyFence(m_fence);
}
