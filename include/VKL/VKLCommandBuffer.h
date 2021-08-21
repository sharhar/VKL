#ifndef VKLCommandBuffer_h
#define VKLCommandBuffer_h

#include "VKL_base.h"

class VKLCommandBuffer : public VKLHandle<VkCommandBuffer> {
public:
	VKLCommandBuffer(const VKLQueue* queue);

	VkCommandPool pool();
	
	void bufferBarrier(VKLBuffer* buffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
	
	void copyBuffer(VKLBuffer* dst, VKLBuffer* src, VkBufferCopy bufferCopy);
	
	void begin();
	void end();
	void reset();
	
	void nextSubpass(VkSubpassContents contents);
	void endRenderPass();

	void destroy();
private:
	VkCommandPool m_pool;
	
	const VKLQueue* m_queue;
	const VKLDevice* m_device;
};

#endif /* VKLCommandBuffer_h */
