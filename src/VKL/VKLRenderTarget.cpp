#include <VKL/VKL.h>

void VKLRenderTarget::initRenderTarget(VKLDevice* device, VkAttachmentDescription* attachmentDescriptions, uint32_t attachmentDescriptionCount, VkSubpassDescription* subpasses, uint32_t subpassCount) {
	VkRenderPassCreateInfo renderPassCreateInfo;
	memset(&renderPassCreateInfo, 0, sizeof(VkRenderPassCreateInfo));
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = attachmentDescriptionCount;
	renderPassCreateInfo.pAttachments = attachmentDescriptions;
	renderPassCreateInfo.subpassCount = subpassCount;
	renderPassCreateInfo.pSubpasses = subpasses;
	
	VK_CALL(device->vk.CreateRenderPass(device->handle(), &renderPassCreateInfo, device->allocationCallbacks(), &m_renderPass));
	
	m_clearValueCount = attachmentDescriptionCount;
	
	m_clearValues = (VkClearValue*)malloc(sizeof(VkClearValue) * m_clearValueCount);
}

void VKLRenderTarget::destroyRenderTarget(VKLDevice* device) {
	VK_CALL(device->vk.DestroyRenderPass(device->handle(), m_renderPass, device->allocationCallbacks()));
}

void VKLRenderTarget::setRenderArea(VkRect2D area) {
	m_renderArea = area;
}

void VKLRenderTarget::setClearValue(VkClearValue clearValue, uint32_t clearValueIndex) {
	m_clearValues[clearValueIndex] = clearValue;
}

VkRenderPass VKLRenderTarget::getRenderPass() {
	return m_renderPass;
}

VkRect2D VKLRenderTarget::getRenderArea() {
	return m_renderArea;
}

VkClearValue* VKLRenderTarget::getClearValues() {
	return m_clearValues;
}

uint32_t VKLRenderTarget::getClearValueCount() {
	return m_clearValueCount;
}
