#ifndef VKLQueue_h
#define VKLQueue_h

#include "VKL_base.h"

class VKLQueue : public VKLHandle<VkQueue> {
public:
	VKLQueue();
	void init(const VKLDevice* device, VkQueue queue, uint32_t familyIndex);

	uint32_t getFamilyIndex() const;
	const VKLDevice* device() const;
	VKLCommandBuffer* getCmdBuffer() const;
	
	void submit(const VKLCommandBuffer* cmdBuffer, VkFence fence) const;
	void waitIdle() const;
private:
	uint32_t m_familyIndex;
	VKLCommandBuffer* m_cmdBuffer;

	const VKLDevice* m_device;
};

#endif 
