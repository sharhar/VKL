#include <VKL/VKL.h>

VKLSwapChain::VKLSwapChain() : VKLCreator<VKLSwapChainCreateInfo>("VKLSwapChain") {
	
}

VKLSwapChain::VKLSwapChain(const VKLSwapChainCreateInfo& createInfo) : VKLCreator<VKLSwapChainCreateInfo>("VKLSwapChain")  {
	this->create(createInfo);
}

void VKLSwapChain::_create(const VKLSwapChainCreateInfo& createInfo) {
	m_device = createInfo.m_queue->device();
	m_queue = createInfo.m_queue;

	VK_CALL(m_device->vk.CreateSwapchainKHR(m_device->handle(), &createInfo.m_createInfo, m_device->allocationCallbacks(), &m_handle));

	VkImage* presentImages = NULL;

	m_device->vk.GetSwapchainImagesKHR(m_device->handle(), m_handle, &m_swapChainImageCount, NULL);
	presentImages = (VkImage*)malloc(sizeof(VkImage) * m_swapChainImageCount);
	m_device->vk.GetSwapchainImagesKHR(m_device->handle(), m_handle, &m_swapChainImageCount, presentImages);

	m_swapChainImages = new VKLImage[m_swapChainImageCount];
	m_swapChainImageViews = new VKLImageView[m_swapChainImageCount];

	for(int i = 0; i < m_swapChainImageCount; i++) { // TODO: init these VKLImage objects in a less hacky way
		m_swapChainImages[i].m_device = m_device;
		m_swapChainImages[i].m_handle = presentImages[i];
		m_swapChainImages[i].m_format = createInfo.m_createInfo.imageFormat;
		m_swapChainImages[i].initBarrier(VK_IMAGE_LAYOUT_UNDEFINED);
		
		m_swapChainImageViews[i].create(VKLImageViewCreateInfo().image(&m_swapChainImages[i]));
	}

	free(presentImages);

	VKLCommandBuffer* cmdBuffer = m_queue->getCmdBuffer();
	
	cmdBuffer->begin();
	
	for (int i = 0; i < m_swapChainImageCount; i++) {
		m_swapChainImages[i].setNewAccessMask(VK_ACCESS_MEMORY_READ_BIT);
		m_swapChainImages[i].setNewLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		cmdBuffer->imageBarrier(&m_swapChainImages[i], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
		m_swapChainImages[i].resetBarrier();
	}
	
	cmdBuffer->end();
	
	VkFence fence = m_device->createFence(0);
	
	m_queue->submit(cmdBuffer, fence);
	
	VkAttachmentDescription passAttachments[2];
	memset(passAttachments, 0, sizeof(VkAttachmentDescription) * 2);
	passAttachments[0].format = createInfo.m_createInfo.imageFormat;
	passAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	passAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	passAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	passAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	passAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	passAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	passAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	passAttachments[1].format = VK_FORMAT_D32_SFLOAT;
	passAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	passAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	passAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	passAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	passAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	passAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	passAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference;
	memset(&colorAttachmentReference, 0, sizeof(VkAttachmentReference));
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference;
	memset(&depthAttachmentReference, 0, sizeof(VkAttachmentReference));
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass;
	memset(&subpass, 0, sizeof(VkSubpassDescription));
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	subpass.pDepthStencilAttachment = NULL;//&depthAttachmentReference;
	
	initRenderTarget(m_device, passAttachments, 1, &subpass, 1);
	
	VkImageView frameBufferAttachments[2];
	frameBufferAttachments[0] = VK_NULL_HANDLE;
	frameBufferAttachments[1] = VK_NULL_HANDLE;
	
	VkFramebufferCreateInfo frameBufferCreateInfo;
	memset(&frameBufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.renderPass = m_renderPass;
	frameBufferCreateInfo.attachmentCount = 1;
	frameBufferCreateInfo.pAttachments = frameBufferAttachments;
	frameBufferCreateInfo.width = createInfo.m_createInfo.imageExtent.width;
	frameBufferCreateInfo.height = createInfo.m_createInfo.imageExtent.height;
	frameBufferCreateInfo.layers = 1;
	
	m_frameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * m_swapChainImageCount);
	
	for(int i = 0; i < m_swapChainImageCount; i++) {
		frameBufferAttachments[0] = m_swapChainImageViews[i].handle();
		
		VK_CALL(m_device->vk.CreateFramebuffer(m_device->handle(), &frameBufferCreateInfo, m_device->allocationCallbacks(), &m_frameBuffers[i]));
	}
	
	m_renderArea.offset.x = 0;
	m_renderArea.offset.y = 0;
	m_renderArea.extent = createInfo.m_createInfo.imageExtent;
	
	m_device->waitForFence(fence);
	m_device->destroyFence(fence);
	
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;
	
	VK_CALL(m_device->vk.CreateSemaphore(m_device->handle(), &semaphoreCreateInfo, m_device->allocationCallbacks(), &m_presentSemaphore));
	VK_CALL(m_device->vk.AcquireNextImageKHR(m_device->handle(), m_handle, UINT64_MAX, m_presentSemaphore, VK_NULL_HANDLE, &m_currentImgIndex));
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
	
	m_device->vk.DestroySemaphore(m_device->handle(), m_presentSemaphore, m_device->allocationCallbacks());
	
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;
	
	VK_CALL(m_device->vk.CreateSemaphore(m_device->handle(), &semaphoreCreateInfo, m_device->allocationCallbacks(), &m_presentSemaphore));
	VK_CALL(m_device->vk.AcquireNextImageKHR(m_device->handle(), m_handle, UINT64_MAX, m_presentSemaphore, VK_NULL_HANDLE, &m_currentImgIndex));
}

void VKLSwapChain::_destroy() {
	destroyRenderTarget();
	
	m_device->vk.DestroySemaphore(m_device->handle(), m_presentSemaphore, m_device->allocationCallbacks());
	
	for (int i = 0; i < m_swapChainImageCount; i++) {
		m_swapChainImageViews[i].destroy();
		m_device->vk.DestroyFramebuffer(m_device->handle(), m_frameBuffers[i], m_device->allocationCallbacks());
	}

	delete[] m_swapChainImageViews;
	delete[] m_swapChainImages;

	m_device->vk.DestroySwapchainKHR(m_device->handle(), m_handle, m_device->allocationCallbacks());
}

VkFramebuffer VKLSwapChain::getCurrentFramebuffer() {
	return m_frameBuffers[m_currentImgIndex];
}

void VKLSwapChain::preRenderCallback(VKLCommandBuffer* cmdBuffer) {
	m_swapChainImages[m_currentImgIndex].setNewAccessMask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	m_swapChainImages[m_currentImgIndex].setNewLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	cmdBuffer->imageBarrier(&m_swapChainImages[m_currentImgIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	m_swapChainImages[m_currentImgIndex].resetBarrier();
	
}

void VKLSwapChain::postRenderCallback(VKLCommandBuffer* cmdBuffer) {
	m_swapChainImages[m_currentImgIndex].setNewAccessMask(VK_ACCESS_MEMORY_READ_BIT);
	m_swapChainImages[m_currentImgIndex].setNewLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	cmdBuffer->imageBarrier(&m_swapChainImages[m_currentImgIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
	m_swapChainImages[m_currentImgIndex].resetBarrier();
}

VKLSwapChainCreateInfo::VKLSwapChainCreateInfo() {
	m_queue = NULL;
	
	memset(&m_createInfo, 0, sizeof(VkSwapchainCreateInfoKHR));
	m_createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	m_createInfo.pNext = NULL;
	m_createInfo.flags = 0;
	m_createInfo.surface = VK_NULL_HANDLE;
	m_createInfo.minImageCount = 2;
	m_createInfo.imageFormat = VK_FORMAT_UNDEFINED;
	m_createInfo.imageExtent.width = -1;
	m_createInfo.imageExtent.height = -1;
	m_createInfo.imageArrayLayers = 1;
	m_createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	m_createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_createInfo.queueFamilyIndexCount = 0;
	m_createInfo.pQueueFamilyIndices = NULL;
	m_createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	m_createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	m_createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	m_createInfo.clipped = VK_TRUE;
	m_createInfo.oldSwapchain = VK_NULL_HANDLE;
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::queue(const VKLQueue* queue) {
	m_queue = queue;
	
	return invalidate();
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::surface(VkSurfaceKHR surface) {
	m_createInfo.surface = surface;
	
	return invalidate();
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::size(VkExtent2D size) {
	m_createInfo.imageExtent = size;
	
	return invalidate();
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::imageFormat(VkFormat format) {
	m_createInfo.imageFormat = format;
	
	return invalidate();
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::imageCount(uint32_t imageCount) {
	m_createInfo.minImageCount = imageCount;
	
	return invalidate();
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::preTransform(VkSurfaceTransformFlagBitsKHR preTransform) {
	m_createInfo.preTransform = preTransform;
	
	return invalidate();
}

VKLSwapChainCreateInfo& VKLSwapChainCreateInfo::presentMode(VkPresentModeKHR presentMode) {
	m_createInfo.presentMode = presentMode;
	
	return invalidate();
}

bool VKLSwapChainCreateInfo::_validate() {
	if(m_queue == NULL) {
		printf("VKL Validation Error: VKLSwapChainCreateInfo::queue was not set!\n");
		return false;
	}

	if (m_createInfo.surface == VK_NULL_HANDLE) {
		printf("VKL Validation Error: VKLSwapChainCreateInfo::surface was not set!\n");
		return false;
	}
	
	const VKLPhysicalDevice* physicalDevice = m_queue->device()->physical();
	
	if(!physicalDevice->getSurfaceSupport(m_createInfo.surface, m_queue->getFamilyIndex())) {
		return false;
	}
	
	std::vector<VkSurfaceFormatKHR> surfaceFormats = physicalDevice->getSurfaceFormats(m_createInfo.surface);
	
	VkBool32 foundFormat = VK_FALSE;
	
	for(VkSurfaceFormatKHR surfaceFormat : surfaceFormats) {
		if(surfaceFormat.format == m_createInfo.imageFormat) {
			m_createInfo.imageColorSpace = surfaceFormat.colorSpace;
			foundFormat = VK_TRUE;
		}
	}
	
	if(!foundFormat) {
		if (m_createInfo.imageFormat != VK_FORMAT_UNDEFINED) {
			printf("VKL Validation Warning: Requested format is not supported by the surface!\n");
			printf("                        Reverting to first supported format from vkGetPhysicalDeviceSurfaceFormatsKHR (VkFormat = %d)\n", surfaceFormats[0].format);
		}

		m_createInfo.imageFormat = surfaceFormats[0].format;
		m_createInfo.imageColorSpace = surfaceFormats[0].colorSpace;
	}
	
	VkSurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice->getSurfaceCapabilities(m_createInfo.surface);

	if (m_createInfo.minImageCount < surfaceCapabilities.minImageCount) {
		m_createInfo.minImageCount = surfaceCapabilities.minImageCount;
	}
	else if (surfaceCapabilities.maxImageCount != 0 && m_createInfo.minImageCount > surfaceCapabilities.maxImageCount) {
		m_createInfo.minImageCount = surfaceCapabilities.maxImageCount;
	}
	
	if(surfaceCapabilities.currentExtent.width == -1) {
		if(m_createInfo.imageExtent.width == -1) {
			m_createInfo.imageExtent.width = 800;
		}
		
		if(m_createInfo.imageExtent.width < surfaceCapabilities.minImageExtent.width) {
			m_createInfo.imageExtent.width = surfaceCapabilities.minImageExtent.width;
		}
		
		if(m_createInfo.imageExtent.width > surfaceCapabilities.maxImageExtent.width) {
			m_createInfo.imageExtent.width = surfaceCapabilities.maxImageExtent.width;
		}
	} else {
		m_createInfo.imageExtent.width = surfaceCapabilities.currentExtent.width;
	}
	
	if(surfaceCapabilities.currentExtent.height == -1) {
		if(m_createInfo.imageExtent.height == -1) {
			m_createInfo.imageExtent.height = 600;
		}
		
		if(m_createInfo.imageExtent.height < surfaceCapabilities.minImageExtent.height) {
			m_createInfo.imageExtent.height = surfaceCapabilities.minImageExtent.height;
		}
		
		if(m_createInfo.imageExtent.height > surfaceCapabilities.maxImageExtent.height) {
			m_createInfo.imageExtent.height = surfaceCapabilities.maxImageExtent.height;
		}
	} else {
		m_createInfo.imageExtent.height = surfaceCapabilities.currentExtent.height;
	}
	
	//TODO: finish checking for support of parameters specified by surfaceCapabilities
	
	std::vector<VkPresentModeKHR> presentModes = physicalDevice->getSurfacePresentModes(m_createInfo.surface);
	
	VkBool32 foundPresentMode = VK_FALSE;
	
	for(VkPresentModeKHR presentMode : presentModes) {
		if(presentMode == m_createInfo.presentMode) {
			foundPresentMode = VK_TRUE;
		}
	}
	
	if(!foundPresentMode) {
		m_createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}
	
	return true;
}
