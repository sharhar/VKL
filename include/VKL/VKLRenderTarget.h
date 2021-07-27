//
//  VKLRenderTarget.h
//  VKL
//
//  Created by Shahar Sandhaus on 7/21/21.
//

#ifndef VKLRenderTarget_h
#define VKLRenderTarget_h

#include "VKL_base.h"

class VKLRenderTarget {
public:
	void setRenderArea(VkRect2D area);
	void setClearValue(VkClearValue clearValue, uint32_t clearValueIndex);
	
	void beingRender(VKLCommandBuffer* cmdBuffer);
	void endRender(VKLCommandBuffer* cmdBuffer);
	
protected:
	void initRenderTarget(VKLDevice* device, VkAttachmentDescription* attachmentDescriptions, uint32_t attachmentDescriptionCount, VkSubpassDescription* subpasses, uint32_t subpassCount);
	
	virtual VkFramebuffer getCurrentFramebuffer() = 0;
	virtual void preRenderCallback(VKLCommandBuffer* cmdBuffer) = 0;
	virtual void postRenderCallback(VKLCommandBuffer* cmdBuffer) = 0;
	
	void destroyRenderTarget();
	
	VkRenderPass m_renderPass;
	VkRect2D m_renderArea;
	
	uint32_t m_clearValueCount;
	VkClearValue* m_clearValues;
private:
	VKLDevice* m_device;
	
};

#endif /* VKLRenderTarget_h */
