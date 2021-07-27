#ifndef VKLSwapChain_h
#define VKLSwapChain_h

#include "VKL_base.h"

#include "VKLRenderTarget.h"

typedef struct VKLSwapChainCreateInfo {
	VKLDevice* device;
	VKLQueue* queue;
	VkSurfaceKHR surface;
} VKLSwapChainCreateInfo;

class VKLSwapChain : public VKLRenderTarget, public VKLHandle<VkSwapchainKHR>, public VKLBuilder<VKLSwapChainCreateInfo> {
public:
	VKLSwapChain();
	void present();
	
	bool buildable() { return true; }
	
	void destroy();
	
private:
	VkExtent2D m_size;

	VKLImage* m_swapChainImages;
	VkFramebuffer* m_frameBuffers;
	uint32_t m_swapChainImageCount;
	
	uint32_t m_currentImgIndex;
	
	VkSemaphore m_presentSemaphore;

	VKLDevice* m_device;
	VKLQueue* m_queue;
	
	VkFramebuffer getCurrentFramebuffer();
	void preRenderCallback(VKLCommandBuffer* cmdBuffer);
	void postRenderCallback(VKLCommandBuffer* cmdBuffer);
	
	void _build();
};

#endif
