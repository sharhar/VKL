#ifndef VKLCommandBuffer_h
#define VKLCommandBuffer_h

#include "VKL_base.h"

class VKLCommandBuffer {
public:
	VKLCommandBuffer(VKLQueue* queue);

	VkCommandPool pool();
	VkCommandBuffer handle();
	
	void begin();
	void end();
	void reset();

	void destroy();
private:
	VkCommandPool m_pool;
	VkCommandBuffer m_handle;
	
	VKLQueue* m_queue;
	VKLDevice* m_device;
};

#endif /* VKLCommandBuffer_h */
