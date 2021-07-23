#ifndef VKLQueue_h
#define VKLQueue_h

#include "VKL_base.h"

typedef struct VKLQueueCreateInfo {
	VKLQueueCreateInfo(uint32_t typeCount, VKLPhysicalDevice* physicalDevice);

	int32_t getQueueFamilyIndex(VkQueueFlags supportFlags, VkQueueFlags excludeFlags);
	int32_t getQueueFamilyIndexWithSurfaceSupport(VkQueueFlags supportFlags, VkQueueFlags excludeFlags, VKLSurface& surface);

	void setQueueType(uint32_t typeIndex, uint32_t count, uint32_t queueFamilyIndex);
	void setQueuePriorities(uint32_t typeIndex, float* priorityList);

	~VKLQueueCreateInfo();

	VKLPhysicalDevice* physicalDevice;
	uint32_t typeCount;
	float** priorities;
	VkDeviceQueueCreateInfo* createInfos;
} VKLQueueCreateInfo;

class VKLQueue {
public:
	VKLQueue();
	void init(VKLDevice* device, VkQueue queue, uint32_t familyIndex);

	uint32_t getFamilyIndex();
	VkQueue handle();
	VKLDevice* getDevice();
	VKLCommandBuffer* getCmdBuffer();
	
	void submit(VKLCommandBuffer* cmdBuffer, VkFence fence);
	void waitIdle();

	//typedef VkResult (VKAPI_PTR *PFN_vkQueueSubmit)(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence);
	//typedef VkResult(VKAPI_PTR* PFN_vkQueueWaitIdle)(VkQueue queue);
	//typedef VkResult (VKAPI_PTR *PFN_vkQueueBindSparse)(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence);
private:
	VkQueue m_handle;
	uint32_t m_familyIndex;
	VKLCommandBuffer* m_cmdBuffer;

	VKLDevice* m_device;
};

#endif 
