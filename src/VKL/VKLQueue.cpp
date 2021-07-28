#include <VKL/VKL.h>

VKLQueue::VKLQueue() {
	m_handle = VK_NULL_HANDLE;
	m_device = NULL;
	m_familyIndex = -1;
}

void VKLQueue::init(const VKLDevice* device, VkQueue queue, uint32_t familyIndex) {
	m_handle = queue;
	m_device = device;
	m_familyIndex = familyIndex;
	
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

void VKLQueue::submit(const VKLCommandBuffer* cmdBuffer, VkFence fence) const {
	VkCommandBuffer cmdBuffHandle = cmdBuffer->handle();
	
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffHandle;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
	
	VK_CALL(m_device->vk.QueueSubmit(m_handle, 1, &submitInfo, fence));
}

void VKLQueue::waitIdle() const {
	m_device->vk.QueueWaitIdle(m_handle);
}
