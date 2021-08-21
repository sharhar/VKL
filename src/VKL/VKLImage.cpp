#include <VKL/VKL.h>

VKLImage::VKLImage() : VKLCreator<VKLImageCreateInfo>("VKLImage") {
	m_device = NULL;
	m_allocation = VK_NULL_HANDLE;
	m_handle = VK_NULL_HANDLE;
}

VKLImage::VKLImage(const VKLImageCreateInfo& createInfo) : VKLCreator<VKLImageCreateInfo>("VKLImage") {
	m_device = NULL;
	m_allocation = VK_NULL_HANDLE;
	m_handle = VK_NULL_HANDLE;
	create(createInfo);
}

void VKLImage::_create(const VKLImageCreateInfo& createInfo) {
	_create(createInfo, VK_NULL_HANDLE);
}


void VKLImage::_create(const VKLImageCreateInfo& createInfo, VkImage handle) {
	m_device = createInfo.m_device;
	m_format = createInfo.m_imageCreateInfo.format;
	m_size = createInfo.m_imageCreateInfo.extent;
	
	if(handle == VK_NULL_HANDLE) {
		vmaCreateImage(m_device->allocator(), &createInfo.m_imageCreateInfo, &createInfo.m_allocationCreateInfo, &m_handle, &m_allocation, NULL);
	} else {
		m_handle = handle;
	}
	

	initBarrier(createInfo.m_imageCreateInfo.initialLayout);
}

void VKLImage::initBarrier(VkImageLayout layout) {
	m_memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	m_memoryBarrier.pNext = NULL;
	m_memoryBarrier.srcAccessMask = 0;
	m_memoryBarrier.dstAccessMask = 0;
	m_memoryBarrier.oldLayout = layout;
	m_memoryBarrier.newLayout = layout;
	m_memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_memoryBarrier.image = m_handle;
	m_memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	m_memoryBarrier.subresourceRange.baseMipLevel = 0;
	m_memoryBarrier.subresourceRange.levelCount = 1;
	m_memoryBarrier.subresourceRange.baseArrayLayer = 0;
	m_memoryBarrier.subresourceRange.layerCount = 1;
}

VkAccessFlags VKLImage::accessMask() const {
	return m_memoryBarrier.dstAccessMask;
}

VkImageLayout VKLImage::layout() const {
	return m_memoryBarrier.newLayout;
}

VkExtent3D VKLImage::extent() const {
	return m_size;
}

void VKLImage::cmdTransitionBarrier(VKLCommandBuffer* cmdBuffer, VkAccessFlags accessMask, VkImageLayout layout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
	m_memoryBarrier.dstAccessMask = accessMask;
	m_memoryBarrier.newLayout = layout;
	
	m_device->vk.CmdPipelineBarrier(cmdBuffer->handle(), srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, &m_memoryBarrier);
	
	m_memoryBarrier.srcAccessMask = m_memoryBarrier.dstAccessMask;
	m_memoryBarrier.oldLayout = m_memoryBarrier.newLayout;
}

VkFormat VKLImage::format() const {
	return m_format;
}

void VKLImage::_destroy() {
	vmaDestroyImage(m_device->allocator(), m_handle, m_allocation);
}

VKLImageCreateInfo::VKLImageCreateInfo() {
	memset(&m_imageCreateInfo, 0, sizeof(VkImageCreateInfo));
	m_imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	m_imageCreateInfo.pNext = NULL;
	m_imageCreateInfo.flags = 0;
	m_imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	m_imageCreateInfo.format = VK_FORMAT_UNDEFINED;
	m_imageCreateInfo.mipLevels = 1;
	m_imageCreateInfo.arrayLayers = 1;
	m_imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	m_imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	m_imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	memset(&m_allocationCreateInfo, 0, sizeof(VmaAllocationCreateInfo));
	m_allocationCreateInfo.flags = 0;
	m_allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
	m_allocationCreateInfo.memoryTypeBits = 0;
	m_allocationCreateInfo.pUserData = NULL;
	m_allocationCreateInfo.pool = VK_NULL_HANDLE;
	m_allocationCreateInfo.requiredFlags = 0;
	m_allocationCreateInfo.preferredFlags = 0;
}

VKLImageCreateInfo& VKLImageCreateInfo::device(const VKLDevice* device) {
	m_device = device;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::imageType(VkImageType type) {
	m_imageCreateInfo.imageType = type;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::format(VkFormat format) {
	m_imageCreateInfo.format = format;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::extent(uint32_t width, uint32_t height, uint32_t depth) {
	m_imageCreateInfo.extent.width = width;
	m_imageCreateInfo.extent.height = height;
	m_imageCreateInfo.extent.depth = depth;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::tiling(VkImageTiling tiling) {
	m_imageCreateInfo.tiling = tiling;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::usage(VkImageUsageFlags usage) {
	m_imageCreateInfo.usage = usage;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::initialLayout(VkImageLayout layout) {
	m_imageCreateInfo.initialLayout = layout;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::allocationFlags(VmaAllocationCreateFlags flags) {
	m_allocationCreateInfo.flags = flags;
	
	return invalidate();
}

VKLImageCreateInfo& VKLImageCreateInfo::memoryUsage(VmaMemoryUsage memoryUsage) {
	m_allocationCreateInfo.usage = memoryUsage;
	
	return invalidate();
}

bool VKLImageCreateInfo::_validate() {
	if(m_device == NULL) {
		printf("VKL Validation Error: VKLImageCreateInfo::device is not set!\n");
		return false;
	}
	
	if (m_imageCreateInfo.extent.width == 0 || m_imageCreateInfo.extent.height == 0 || m_imageCreateInfo.extent.depth == 0) {
		printf("VKL Validation Error: VKLImageCreateInfo::extent is zero on at least one dimension!\n");
		return false;
	}

	if (m_imageCreateInfo.format == VK_FORMAT_UNDEFINED) {
		printf("VKL Validation Error: VKLImageCreateInfo::format is not set!\n");
		return false;
	}

	if (m_imageCreateInfo.usage == 0) {
		printf("VKL Validation Error: VKLImageCreateInfo::usage is not set!\n");
		return false;
	}
	
	return true;
}
