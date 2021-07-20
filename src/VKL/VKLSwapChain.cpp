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


}

VkSwapchainKHR VKLSwapChain::handle() {
	return m_handle;
}

void VKLSwapChain::present() {
	
}

void VKLSwapChain::destroy() {
	for (int i = 0; i < m_swapChainImageCount; i++) {
		m_swapChainImages[i].destroy();
	}

	delete[] m_swapChainImages;

	m_device->vk.DestroySwapchainKHR(m_device->handle(), m_handle, m_device->allocator());
}