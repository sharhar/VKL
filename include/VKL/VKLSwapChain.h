#ifndef VKLSwapChain_h
#define VKLSwapChain_h

#include "VKL_base.h"

#include "VKLRenderTarget.h"

class VKLSwapChain : public VKLRenderTarget {
public:
	VKLSwapChain(VKLDevice* device, VKLSurface* surface, VKLQueue* queue);
	VkSwapchainKHR handle();
	void present();
	void destroy();

	VkFramebuffer getCurrentFramebuffer();
	void preRenderCallback(VKLCommandBuffer* cmdBuffer);
	void postRenderCallback(VKLCommandBuffer* cmdBuffer);
private:
	VkSwapchainKHR m_handle;

	VkExtent2D m_size;

	VKLImage* m_swapChainImages;
	VkFramebuffer* m_frameBuffers;
	uint32_t m_swapChainImageCount;
	
	uint32_t m_currentImgIndex;
	
	VkSemaphore m_presentSemaphore;

	VKLDevice* m_device;
	VKLQueue* m_queue;
};

#endif
