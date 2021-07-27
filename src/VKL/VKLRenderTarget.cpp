#include <VKL/VKL.h>

void VKLRenderTarget::initRenderTarget(VKLDevice* device, VkAttachmentDescription* attachmentDescriptions, uint32_t attachmentDescriptionCount, VkSubpassDescription* subpasses, uint32_t subpassCount) {
	m_device = device;
	
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

void VKLRenderTarget::destroyRenderTarget() {
	VK_CALL(m_device->vk.DestroyRenderPass(m_device->handle(), m_renderPass, m_device->allocationCallbacks()));
}

void VKLRenderTarget::setRenderArea(VkRect2D area) {
	m_renderArea = area;
}

void VKLRenderTarget::setClearValue(VkClearValue clearValue, uint32_t clearValueIndex) {
	m_clearValues[clearValueIndex] = clearValue;
}

void VKLRenderTarget::beingRender(VKLCommandBuffer* cmdBuffer) {
	preRenderCallback(cmdBuffer);
	
	VkRenderPassBeginInfo renderPassBeginInfo;
	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = m_renderPass;
	renderPassBeginInfo.framebuffer = getCurrentFramebuffer();
	renderPassBeginInfo.renderArea = m_renderArea;
	renderPassBeginInfo.clearValueCount = m_clearValueCount;
	renderPassBeginInfo.pClearValues = m_clearValues;
	
	m_device->vk.CmdBeginRenderPass(cmdBuffer->handle(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VKLRenderTarget::endRender(VKLCommandBuffer* cmdBuffer) {
	m_device->vk.CmdEndRenderPass(cmdBuffer->handle());
	
	postRenderCallback(cmdBuffer);
}
