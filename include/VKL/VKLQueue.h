#ifndef VKLQueue_h
#define VKLQueue_h

#include "VKL_base.h"

class VKLQueue : public VKLHandle<VkQueue> {
public:
	VKLQueue();
	void init(VKLDevice* device, VkQueue queue, uint32_t familyIndex);

	uint32_t getFamilyIndex();
	VKLDevice* getDevice();
	VKLCommandBuffer* getCmdBuffer();
	
	void submit(VKLCommandBuffer* cmdBuffer, VkFence fence);
	void waitIdle();
private:
	uint32_t m_familyIndex;
	VKLCommandBuffer* m_cmdBuffer;

	VKLDevice* m_device;
};

#endif 
