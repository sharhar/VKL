#include <VKL/VKLImage.h>
#include <VKL/VKLDevice.h>

VKLImage::VKLImage() {
	m_device = NULL;
	m_allocator = VK_NULL_HANDLE;
	m_allocation = VK_NULL_HANDLE;
	m_handle = VK_NULL_HANDLE;
	m_view = VK_NULL_HANDLE;
}

VKLImage::VKLImage(VKLImageCreateInfo* createInfo, VKLDevice* device) {
	m_device = NULL;
	m_allocator = VK_NULL_HANDLE;
	m_allocation = VK_NULL_HANDLE;
	m_handle = VK_NULL_HANDLE;
	m_view = VK_NULL_HANDLE;
	create(createInfo, device);
}

void VKLImage::create(VKLImageCreateInfo* createInfo, VKLDevice* device) {
	m_device = device;

	if (createInfo->allocator != VK_NULL_HANDLE) {
		m_allocator = createInfo->allocator;

		m_handle = VK_NULL_HANDLE;
		m_allocation = VK_NULL_HANDLE;
	}
	else {
		m_handle = createInfo->imgHandle;
		m_allocation = VK_NULL_HANDLE;
		m_allocator = VK_NULL_HANDLE;
	}

	VkImageViewCreateInfo imageViewCreateInfo;
	memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = NULL;
	imageViewCreateInfo.image = m_handle;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = createInfo->format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VK_CALL(device->vk.CreateImageView(device->handle(), &imageViewCreateInfo, device->allocator(), &m_view));
}

VkImage VKLImage::handle() {
	return m_handle;
}

VkImageView VKLImage::view() {
	return m_view;
}

void VKLImage::destroy() {
	m_device->vk.DestroyImageView(m_device->handle(), m_view, m_device->allocator());

	if (m_allocation != VK_NULL_HANDLE) {
		//vmaDestroyImage(m_allocator, m_handle, m_allocation);
	}
}

VKLImageCreateInfo::VKLImageCreateInfo(VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage) {
	this->format = format;
	this->width = width;
	this->height = height;
	this->depth = 1;
	this->usage = usage;
	
	this->imageType = VK_IMAGE_TYPE_2D;
	this->mipLevels = 1;
	this->arrayLayers = 1;
	this->sampleCount = VK_SAMPLE_COUNT_1_BIT;
	this->tiling = VK_IMAGE_TILING_LINEAR;

	this->allocator = VK_NULL_HANDLE;
	this->imgHandle = VK_NULL_HANDLE;
}
