#ifndef VKLSwapChain_h
#define VKLSwapChain_h

#include "VKL_base.h"

#include "VKLRenderTarget.h"

class VKLSwapChainCreateInfo : public VKLCreateInfo<VKLSwapChainCreateInfo> {
public:
	VKLSwapChainCreateInfo();
	
	VKLSwapChainCreateInfo& queue(const VKLQueue& queue);
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

class VKLSwapChain : public VKLRenderTarget, public VKLHandle<VkSwapchainKHR>, public VKLCreator<VKLSwapChainCreateInfo> {
public:
	VKLSwapChain();
	VKLSwapChain(const VKLSwapChainCreateInfo& createInfo);
	
	void present();
private:
	VKLImage* m_swapChainImages;
	VkFramebuffer* m_frameBuffers;
	uint32_t m_swapChainImageCount;
	
	uint32_t m_currentImgIndex;
	
	VkSemaphore m_presentSemaphore;

	const VKLDevice* m_device;
	const VKLQueue* m_queue;
	
	VkFramebuffer getCurrentFramebuffer();
	void preRenderCallback(VKLCommandBuffer* cmdBuffer);
	void postRenderCallback(VKLCommandBuffer* cmdBuffer);
	
	void _destroy();
	void _create(const VKLSwapChainCreateInfo& createInfo);
};

#endif
