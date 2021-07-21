#ifndef VKLSwapChain_h
#define VKLSwapChain_h

#include "VKL_base.h"

class VKLSwapChain {
public:
	VKLSwapChain(VKLDevice* device, VKLSurface* surface, VKLQueue* queue);
	VkSwapchainKHR handle();
	void present();
	void destroy();

private:
	VkSwapchainKHR m_handle;

	VkExtent2D m_size;

	VKLImage* m_swapChainImages;
	uint32_t m_swapChainImageCount;
	
	uint32_t m_currentImgIndex;
	
	VkSemaphore m_presentSemaphore;

	VKLDevice* m_device;
	VKLQueue* m_queue;
};

#endif
