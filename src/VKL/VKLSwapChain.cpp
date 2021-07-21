#include <VKL/VKL.h>

VKLSwapChain::VKLSwapChain(VKLDevice* device, VKLSurface* surface, VKLQueue* queue) {
	m_device = device;
	m_queue = queue;
	std::vector<VkSurfaceFormatKHR> surfaceFormats = surface->getFormats(&device->physical());

	VkFormat colorFormat;
	if (surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		colorFormat = VK_FORMAT_B8G8R8_UNORM;
	}
	else {
		colorFormat = surfaceFormats[0].format;
	}

	VkColorSpaceKHR colorSpace;
	colorSpace = surfaceFormats[0].colorSpace;

	VkSurfaceCapabilitiesKHR surfaceCapabilities = surface->getCapabilities(&device->physical());

	uint32_t desiredImageCount = 2;
	if (desiredImageCount < surfaceCapabilities.minImageCount) {
		desiredImageCount = surfaceCapabilities.minImageCount;
	}
	else if (surfaceCapabilities.maxImageCount != 0 &&
		desiredImageCount > surfaceCapabilities.maxImageCount) {
		desiredImageCount = surfaceCapabilities.maxImageCount;
	}

	m_size = surface->getSize();

	VkExtent2D surfaceResolution = surfaceCapabilities.currentExtent;
	if (surfaceResolution.width == -1) {
		surfaceResolution = m_size;
	}
	else {
		m_size = surfaceResolution;
	}

	VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}

	std::vector<VkPresentModeKHR> presentModes = surface->getPresentModes(&device->physical());

	VkPresentModeKHR presentationMode = VK_PRESENT_MODE_FIFO_KHR;

	/*
	if (!vSync) {
		for (uint32_t i = 0; i < presentModeCount; ++i) {
			if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				presentationMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				break;
			}
		}
	}
	*/

	VkSwapchainCreateInfoKHR swapChainCreateInfo;
	memset(&swapChainCreateInfo, 0, sizeof(VkSwapchainCreateInfoKHR));
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface->handle();
	swapChainCreateInfo.minImageCount = desiredImageCount;
	swapChainCreateInfo.imageFormat = colorFormat;
	swapChainCreateInfo.imageColorSpace = colorSpace;
	swapChainCreateInfo.imageExtent = surfaceResolution;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.preTransform = preTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = presentationMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_CALL(device->vk.CreateSwapchainKHR(device->handle(), &swapChainCreateInfo, device->allocator(), &m_handle));

	VkImage* presentImages = NULL;

	device->vk.GetSwapchainImagesKHR(device->handle(), m_handle, &m_swapChainImageCount, NULL);
	presentImages = (VkImage*)malloc(sizeof(VkImage) * m_swapChainImageCount);
	device->vk.GetSwapchainImagesKHR(device->handle(), m_handle, &m_swapChainImageCount, presentImages);

	m_swapChainImages = new VKLImage[m_swapChainImageCount];

	VKLImageCreateInfo imageCreateInfo(colorFormat, surfaceResolution.width, surfaceResolution.height, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

	for(int i = 0; i < m_swapChainImageCount; i++) {
		imageCreateInfo.imgHandle = presentImages[i];
		m_swapChainImages[i].create(&imageCreateInfo, device);
	}

	free(presentImages);

	VkImageMemoryBarrier* layoutTransitionBarriers = (VkImageMemoryBarrier*)malloc(sizeof(VkImageMemoryBarrier) * m_swapChainImageCount);
	VkImageSubresourceRange resourceRange;
	resourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	resourceRange.baseMipLevel = 0;
	resourceRange.levelCount = 1;
	resourceRange.baseArrayLayer = 0;
	resourceRange.layerCount = 1;
		
	for (int i = 0; i < m_swapChainImageCount; i++) {
		layoutTransitionBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		layoutTransitionBarriers[i].pNext = NULL;
		layoutTransitionBarriers[i].srcAccessMask = 0;
		layoutTransitionBarriers[i].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		layoutTransitionBarriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		layoutTransitionBarriers[i].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		layoutTransitionBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarriers[i].image = m_swapChainImages[i].handle();
		layoutTransitionBarriers[i].subresourceRange = resourceRange;
	}
	
	m_queue->getCmdBuffer()->begin();
	
	m_device->vk.CmdPipelineBarrier(m_queue->getCmdBuffer()->handle(),
									VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
									VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
									0,
									0, NULL,
									0, NULL,
									m_swapChainImageCount, layoutTransitionBarriers);
	
	m_queue->getCmdBuffer()->end();
	
	m_queue->submit(m_queue->getCmdBuffer());
	
	m_queue->waitIdle();
	
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;
	
	VK_CALL(m_device->vk.CreateSemaphore(m_device->handle(), &semaphoreCreateInfo, m_device->allocator(), &m_presentSemaphore));
	VK_CALL(m_device->vk.AcquireNextImageKHR(m_device->handle(), m_handle, UINT64_MAX, m_presentSemaphore, VK_NULL_HANDLE, &m_currentImgIndex));
}

VkSwapchainKHR VKLSwapChain::handle() {
	return m_handle;
}

void VKLSwapChain::present() {
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_presentSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_handle;
	presentInfo.pImageIndices = &m_currentImgIndex;
	presentInfo.pResults = NULL;
	
	VK_CALL(m_device->vk.QueuePresentKHR(m_queue->handle(), &presentInfo));
	
	m_queue->waitIdle();
	
	m_device->vk.DestroySemaphore(m_device->handle(), m_presentSemaphore, m_device->allocator());
	
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;
	
	VK_CALL(m_device->vk.CreateSemaphore(m_device->handle(), &semaphoreCreateInfo, m_device->allocator(), &m_presentSemaphore));
	VK_CALL(m_device->vk.AcquireNextImageKHR(m_device->handle(), m_handle, UINT64_MAX, m_presentSemaphore, VK_NULL_HANDLE, &m_currentImgIndex));
}

void VKLSwapChain::destroy() {
	m_device->vk.DestroySemaphore(m_device->handle(), m_presentSemaphore, m_device->allocator());
	
	for (int i = 0; i < m_swapChainImageCount; i++) {
		m_swapChainImages[i].destroy();
	}

	delete[] m_swapChainImages;

	m_device->vk.DestroySwapchainKHR(m_device->handle(), m_handle, m_device->allocator());
}
