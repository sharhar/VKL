#include <VKL/VKL.h>

VKLSwapChain::VKLSwapChain() : VKLBuilder<VKLSwapChainCreateInfo>("VKLSwapChain") {
	
}

void VKLSwapChain::_build(VKLSwapChainCreateInfo* createInfo) {
	m_device = createInfo->device;
	m_queue = createInfo->queue;
	
	std::vector<VkSurfaceFormatKHR> surfaceFormats = m_device->physical()->getSurfaceFormats(createInfo->surface);

	VkFormat colorFormat;
	if (surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		colorFormat = VK_FORMAT_B8G8R8_UNORM;
	}
	else {
		colorFormat = surfaceFormats[0].format;
	}

	VkColorSpaceKHR colorSpace;
	colorSpace = surfaceFormats[0].colorSpace;

	VkSurfaceCapabilitiesKHR surfaceCapabilities = m_device->physical()->getSurfaceCapabilities(createInfo->surface);

	uint32_t desiredImageCount = 2;
	if (desiredImageCount < surfaceCapabilities.minImageCount) {
		desiredImageCount = surfaceCapabilities.minImageCount;
	}
	else if (surfaceCapabilities.maxImageCount != 0 &&
		desiredImageCount > surfaceCapabilities.maxImageCount) {
		desiredImageCount = surfaceCapabilities.maxImageCount;
	}

	m_size.width = 800;
	m_size.height = 600;

	VkExtent2D surfaceResolution = surfaceCapabilities.currentExtent;
	
	printf("res: %d %d\n", surfaceResolution.width, surfaceResolution.height);
	
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

	std::vector<VkPresentModeKHR> presentModes = m_device->physical()->getSurfacePresentModes(createInfo->surface);

	VkPresentModeKHR presentationMode = VK_PRESENT_MODE_IMMEDIATE_KHR;//VK_PRESENT_MODE_FIFO_KHR;

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
	swapChainCreateInfo.surface = createInfo->surface;
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

	VK_CALL(m_device->vk.CreateSwapchainKHR(m_device->handle(), &swapChainCreateInfo, m_device->allocationCallbacks(), &m_handle));

	VkImage* presentImages = NULL;

	m_device->vk.GetSwapchainImagesKHR(m_device->handle(), m_handle, &m_swapChainImageCount, NULL);
	presentImages = (VkImage*)malloc(sizeof(VkImage) * m_swapChainImageCount);
	m_device->vk.GetSwapchainImagesKHR(m_device->handle(), m_handle, &m_swapChainImageCount, presentImages);

	m_swapChainImages = new VKLImage[m_swapChainImageCount];

	VKLImageCreateInfo imageCreateInfo(colorFormat, surfaceResolution.width, surfaceResolution.height, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	for(int i = 0; i < m_swapChainImageCount; i++) {
		imageCreateInfo.imgHandle = presentImages[i];
		m_swapChainImages[i].create(&imageCreateInfo, m_device);
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
	passAttachments[0].format = colorFormat;
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
	//frameBufferCreateInfo.pAttachments = frameBufferAttachments;
	frameBufferCreateInfo.width = m_size.width;
	frameBufferCreateInfo.height = m_size.height;
	frameBufferCreateInfo.layers = 1;
	
	m_frameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * m_swapChainImageCount);
	
	for(int i = 0; i < m_swapChainImageCount; i++) {
		VkImageView tempView = m_swapChainImages[i].view();
		frameBufferCreateInfo.pAttachments = &tempView;
		
		VK_CALL(m_device->vk.CreateFramebuffer(m_device->handle(), &frameBufferCreateInfo, m_device->allocationCallbacks(), &m_frameBuffers[i]));
	}
	
	m_renderArea.offset.x = 0;
	m_renderArea.offset.y = 0;
	m_renderArea.extent = m_size;
	
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
	
	//m_queue->waitIdle();
	
	m_device->vk.DestroySemaphore(m_device->handle(), m_presentSemaphore, m_device->allocationCallbacks());
	
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;
	
	VK_CALL(m_device->vk.CreateSemaphore(m_device->handle(), &semaphoreCreateInfo, m_device->allocationCallbacks(), &m_presentSemaphore));
	VK_CALL(m_device->vk.AcquireNextImageKHR(m_device->handle(), m_handle, UINT64_MAX, m_presentSemaphore, VK_NULL_HANDLE, &m_currentImgIndex));
}

void VKLSwapChain::destroy() {
	destroyRenderTarget();
	
	m_device->vk.DestroySemaphore(m_device->handle(), m_presentSemaphore, m_device->allocationCallbacks());
	
	for (int i = 0; i < m_swapChainImageCount; i++) {
		m_swapChainImages[i].destroy();
		VK_CALL(m_device->vk.DestroyFramebuffer(m_device->handle(), m_frameBuffers[i], m_device->allocationCallbacks()));
	}

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
