#ifndef VKLSwapChain_h
#define VKLSwapChain_h

#include "VKL_base.h"

class VKLSwapChainCreateInfo : public VKLCreateInfo<VKLSwapChainCreateInfo> {
public:
	VKLSwapChainCreateInfo();
	
	VKLSwapChainCreateInfo& queue(const VKLQueue* queue);
	VKLSwapChainCreateInfo& surface(VkSurfaceKHR surface);
	VKLSwapChainCreateInfo& size(VkExtent2D size);
	VKLSwapChainCreateInfo& imageFormat(VkFormat format);
	VKLSwapChainCreateInfo& imageCount(uint32_t imageCount);
	VKLSwapChainCreateInfo& preTransform(VkSurfaceTransformFlagBitsKHR preTransform);
	VKLSwapChainCreateInfo& presentMode(VkPresentModeKHR presentMode);

private:
	const VKLQueue* m_queue;

	VkSwapchainCreateInfoKHR m_createInfo;

	bool _validate();

	friend class VKLSwapChain;
};

class VKLSwapChain : public VKLHandle<VkSwapchainKHR>, public VKLCreator<VKLSwapChainCreateInfo> {
public:
	VKLSwapChain();
	VKLSwapChain(const VKLSwapChainCreateInfo& createInfo);
	
	VKLImage& getCurrentImage();
	
	void present(const VKLImage* image);
	void present(const VKLImage* image, uint32_t waitSemaphoreCount, const VkSemaphore* waitSemaphores, const VkPipelineStageFlags* pWaitDstStageMask);
private:
	VKLImage* m_swapChainImages;
	uint32_t m_swapChainImageCount;
	
	uint32_t m_currentImgIndex;
	
	VkFence m_fence;
	
	VKLCommandBuffer* m_cmdBuffer;

	const VKLDevice* m_device;
	const VKLQueue* m_queue;
	
	void _destroy();
	void _create(const VKLSwapChainCreateInfo& createInfo);
};

#endif
