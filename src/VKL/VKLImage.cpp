#include <VKL/VKL.h>

VKLImage::VKLImage() : VKLCreator<VKLImageCreateInfo>("VKLImage") {
	m_device = NULL;
	m_allocation = VK_NULL_HANDLE;
	m_handle = VK_NULL_HANDLE;
	m_view = VK_NULL_HANDLE;
}

VKLImage::VKLImage(const VKLImageCreateInfo& createInfo) : VKLCreator<VKLImageCreateInfo>("VKLImage") {
	m_device = NULL;
	m_allocation = VK_NULL_HANDLE;
	m_handle = VK_NULL_HANDLE;
	m_view = VK_NULL_HANDLE;
	create(createInfo);
}

void VKLImage::_create(const VKLImageCreateInfo& createInfo) {
	m_device = createInfo.device;

	if (createInfo.handle != VK_NULL_HANDLE) {
		m_handle = createInfo.handle;
		m_allocation = VK_NULL_HANDLE;
	} else {
		vmaCreateImage(m_device->allocator(), &createInfo.imageCreateInfo, &createInfo.allocationCreateInfo, &m_handle, &m_allocation, NULL);
	}
	
	VkImageViewCreateInfo viewCreateInfo;
	memcpy(&viewCreateInfo, &createInfo.viewCreateInfo, sizeof(VkImageViewCreateInfo));
	viewCreateInfo.image = m_handle;
	
	VK_CALL(m_device->vk.CreateImageView(m_device->handle(), &viewCreateInfo, m_device->allocationCallbacks(), &m_view));
	
	m_memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	m_memoryBarrier.pNext = NULL;
	m_memoryBarrier.srcAccessMask = 0;
	m_memoryBarrier.dstAccessMask = 0;
	m_memoryBarrier.oldLayout = createInfo.imageCreateInfo.initialLayout;
	m_memoryBarrier.newLayout = createInfo.imageCreateInfo.initialLayout;
	m_memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.image = m_handle;
	m_memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	m_memoryBarrier.subresourceRange.baseMipLevel = 0;
	m_memoryBarrier.subresourceRange.levelCount = 1;
	m_memoryBarrier.subresourceRange.baseArrayLayer = 0;
	m_memoryBarrier.subresourceRange.layerCount = 1;
}

void VKLImage::setNewAccessMask(VkAccessFlags accessMask) {
	m_memoryBarrier.dstAccessMask = accessMask;
}

void VKLImage::setNewLayout(VkImageLayout layout) {
	m_memoryBarrier.newLayout = layout;
}

VkImageMemoryBarrier* VKLImage::getMemoryBarrier() {
	return &m_memoryBarrier;
}

void VKLImage::resetBarrier() {
	m_memoryBarrier.srcAccessMask = m_memoryBarrier.dstAccessMask;
	m_memoryBarrier.oldLayout = m_memoryBarrier.newLayout;
}

VkImageView VKLImage::view() {
	return m_view;
}

void VKLImage::_destroy() {
	m_device->vk.DestroyImageView(m_device->handle(), m_view, m_device->allocationCallbacks());

	if (m_allocation != VK_NULL_HANDLE) {
		vmaDestroyImage(m_device->allocator(), m_handle, m_allocation);
	}
}

VKLImageCreateInfo::VKLImageCreateInfo() {
	memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_UNDEFINED;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	memset(&viewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.pNext = NULL;
	viewCreateInfo.flags = 0;
	viewCreateInfo.image = VK_NULL_HANDLE;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = VK_FORMAT_UNDEFINED;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;
	
	memset(&allocationCreateInfo, 0, sizeof(VmaAllocationCreateInfo));
	allocationCreateInfo.flags = 0;
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
	allocationCreateInfo.memoryTypeBits = 0;
	allocationCreateInfo.pUserData = NULL;
	allocationCreateInfo.pool = VK_NULL_HANDLE;
	allocationCreateInfo.requiredFlags = 0;
	allocationCreateInfo.preferredFlags = 0;
	
	handle = VK_NULL_HANDLE;
}

VKLImageCreateInfo& VKLImageCreateInfo::setDevice(const VKLDevice* device) {
	this->device = device;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setHandle(VkImage image) {
	this->handle = image;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setImageType(VkImageType type) {
	imageCreateInfo.imageType = type;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setFormat(VkFormat format) {
	imageCreateInfo.format = format;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setExtent(uint32_t width, uint32_t height, uint32_t depth) {
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setTiling(VkImageTiling tiling) {
	imageCreateInfo.tiling = tiling;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setUsage(VkImageUsageFlags usage) {
	imageCreateInfo.usage = usage;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setInitialLayout(VkImageLayout layout) {
	imageCreateInfo.initialLayout = layout;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setViewType(VkImageViewType type) {
	viewCreateInfo.viewType = type;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setViewFormat(VkFormat format) {
	viewCreateInfo.format = format;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setAllocationFlags(VmaAllocationCreateFlags flags) {
	allocationCreateInfo.flags = flags;
	
	return *this;
}

VKLImageCreateInfo& VKLImageCreateInfo::setMemoryUsage(VmaMemoryUsage memoryUsage) {
	allocationCreateInfo.usage = memoryUsage;
	
	return *this;
}

bool VKLImageCreateInfo::_validate() {
	if(device == NULL) {
		return false;
	}
	
	bool zeroSize = imageCreateInfo.extent.width == 0 || imageCreateInfo.extent.height == 0 || imageCreateInfo.extent.depth == 0;
	bool noFormat = imageCreateInfo.format == VK_FORMAT_UNDEFINED;
	
	if((zeroSize || noFormat || imageCreateInfo.usage == 0) && handle == VK_NULL_HANDLE) {
		return false;
	}
	
	if(noFormat && viewCreateInfo.format == VK_FORMAT_UNDEFINED) {
		return false;
	}
	
	if(viewCreateInfo.format == VK_FORMAT_UNDEFINED) {
		viewCreateInfo.format = imageCreateInfo.format;
	}
	
	return true;
}
