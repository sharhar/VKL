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
	
	VkRenderPass getRenderPass();
	VkRect2D getRenderArea();
	VkClearValue* getClearValues();
	uint32_t getClearValueCount();
	
	virtual VkFramebuffer getCurrentFramebuffer() = 0;
	virtual void preRenderCallback(VKLCommandBuffer* cmdBuffer) = 0;
	virtual void postRenderCallback(VKLCommandBuffer* cmdBuffer) = 0;
protected:
	void initRenderTarget(VKLDevice* device, VkAttachmentDescription* attachmentDescriptions, uint32_t attachmentDescriptionCount, VkSubpassDescription* subpasses, uint32_t subpassCount);
	
	void destroyRenderTarget(VKLDevice* device);
	
	VkRenderPass m_renderPass;
	VkRect2D m_renderArea;
	
	uint32_t m_clearValueCount;
	VkClearValue* m_clearValues;
	
};

#endif /* VKLRenderTarget_h */
