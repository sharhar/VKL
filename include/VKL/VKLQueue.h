#ifndef VKLQueue_h
#define VKLQueue_h

#include "VKL_base.h"

class VKLQueue : public VKLHandle<VkQueue> {
public:
	uint32_t getFamilyIndex() const;
	const VKLDevice* device() const;
	VkFence fence() const;
	VKLCommandBuffer* getCmdBuffer() const;
	
	void submitAndWait(const VKLCommandBuffer* cmdBuffer) const;
	void submitAndWait(const VKLCommandBuffer* cmdBuffer, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores, const VkPipelineStageFlags* pWaitDstStageMask) const;
	
	void submit(const VKLCommandBuffer* cmdBuffer, VkFence fence) const;
	void submit(const VKLCommandBuffer* cmdBuffer, VkFence fence, const VkSemaphore* signalSempahore) const;
	void submit(const VKLCommandBuffer* cmdBuffer, VkFence fence, const VkSemaphore* signalSempahore, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores, const VkPipelineStageFlags* pWaitDstStageMask) const;
	void waitIdle() const;
private:
	VKLQueue();
	void init(const VKLDevice* device, VkQueue queue, uint32_t familyIndex);
	void destroy();
	
	uint32_t m_familyIndex;
	VKLCommandBuffer* m_cmdBuffer;
	
	VkFence m_fence;

	const VKLDevice* m_device;
	
	friend class VKLDevice;
};

#endif 
