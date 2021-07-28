#ifndef VKLCommandBuffer_h
#define VKLCommandBuffer_h

#include "VKL_base.h"

class VKLCommandBuffer : public VKLHandle<VkCommandBuffer> {
public:
	VKLCommandBuffer(VKLQueue* queue);

	VkCommandPool pool();
	
	void imageBarrier(VKLImage* image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
	void bufferBarrier(VKLBuffer* buffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
	
	void copyBuffer(VKLBuffer* dst, VKLBuffer* src, VkBufferCopy bufferCopy);
	
	void begin();
	void end();
	void reset();

	void destroy();
private:
	VkCommandPool m_pool;
	
	VKLQueue* m_queue;
	VKLDevice* m_device;
};

#endif /* VKLCommandBuffer_h */