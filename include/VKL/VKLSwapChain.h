#ifndef VKLSwapChain_h
#define VKLSwapChain_h

#include "VKL_base.h"

#include "VKLRenderTarget.h"

class VKLSwapChainCreateInfo : public VKLCreateInfo {
public:
	VKLSwapChainCreateInfo();
	
	VKLSwapChainCreateInfo& setQueue(const VKLQueue* queue);
	VKLSwapChainCreateInfo& setSurface(const VkSurfaceKHR surface);
	VKLSwapChainCreateInfo& setSize(VkExtent2D size);
	VKLSwapChainCreateInfo& setImageFormat(VkFormat format);
	VKLSwapChainCreateInfo& setImageCount(uint32_t imageCount);
	VKLSwapChainCreateInfo& setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform);
	VKLSwapChainCreateInfo& setPresentMode(VkPresentModeKHR presentMode);
	
	const VKLQueue* queue;
	const VkSurfaceKHR surface;
	VkExtent2D size;
	
	VkSwapchainCreateInfoKHR createInfo;
	
	bool validate();
};

class VKLSwapChain : public VKLRenderTarget, public VKLHandle<VkSwapchainKHR>, public VKLBuilder<VKLSwapChainCreateInfo> {
public:
	VKLSwapChain();
	
	void present();
	
	void destroy();
	
private:
	VkExtent2D m_size;

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
	
	void _build(const VKLSwapChainCreateInfo& createInfo);
};

#endif
