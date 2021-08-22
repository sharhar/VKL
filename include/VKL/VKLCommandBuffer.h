#ifndef VKLCommandBuffer_h
#define VKLCommandBuffer_h

#include "VKL_base.h"

class VKLCommandBuffer : public VKLHandle<VkCommandBuffer> {
public:
	VKLCommandBuffer(const VKLQueue* queue);

	VkCommandPool pool();
	
	void bufferBarrier(VKLBuffer* buffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
	
	void copyBuffer(const VKLBuffer* dst, const VKLBuffer* src, VkBufferCopy bufferCopy);
	void copyImage(const VKLImage* dst, const VKLImage* src, VkImageCopy imageCopy);
	
	void begin();
	void end();
	void reset();
	
	void nextSubpass(VkSubpassContents contents);
	void endRenderPass();
	
	void setViewPort(uint32_t index, uint32_t width, uint32_t height);
	void setViewPort(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	void setViewPort(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t z, uint32_t depth);
	
	void setScissor(uint32_t index, uint32_t width, uint32_t height);
	void setScissor(uint32_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	void destroy();
private:
	VkCommandPool m_pool;
	
	const VKLQueue* m_queue;
	const VKLDevice* m_device;
};

#endif /* VKLCommandBuffer_h */
