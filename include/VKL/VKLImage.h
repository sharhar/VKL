#ifndef VKLImage_h
#define VKLImage_h

#include "VKL_base.h"

class VKLImageCreateInfo : public VKLCreateInfo  {
public:
	VKLImageCreateInfo();
	
	VKLImageCreateInfo& setDevice(const VKLDevice* device);
	
	VKLImageCreateInfo& setHandle(VkImage image);
	
	VKLImageCreateInfo& setImageType(VkImageType type);
	VKLImageCreateInfo& setFormat(VkFormat format);
	VKLImageCreateInfo& setExtent(uint32_t width, uint32_t height, uint32_t depth);
	VKLImageCreateInfo& setTiling(VkImageTiling tiling);
	VKLImageCreateInfo& setUsage(VkImageUsageFlags usage);
	VKLImageCreateInfo& setInitialLayout(VkImageLayout layout);
	
	VKLImageCreateInfo& setViewType(VkImageViewType type);
	VKLImageCreateInfo& setViewFormat(VkFormat format);
	
	VKLImageCreateInfo& setAllocationFlags(VmaAllocationCreateFlags flags);
	VKLImageCreateInfo& setMemoryUsage(VmaMemoryUsage memoryUsage);
	
	bool validate();
	
	VkImageCreateInfo imageCreateInfo;
	VkImageViewCreateInfo viewCreateInfo;
	VmaAllocationCreateInfo allocationCreateInfo;
	VkImage handle;
	
	const VKLDevice* device;
};

class VKLImage : public VKLHandle<VkImage>, public VKLBuilder<VKLImageCreateInfo> {
public:
	VKLImage();
	VKLImage(const VKLImageCreateInfo& createInfo);
	
	void setNewAccessMask(VkAccessFlags accessMask);
	void setNewLayout(VkImageLayout layout);
	
	VkImageMemoryBarrier* getMemoryBarrier();
	void resetBarrier();

	VkImageView view();
	
	void destroy();
private:
	VkImageView m_view;
	VkImageMemoryBarrier m_memoryBarrier;
	VmaAllocation m_allocation;

	const VKLDevice* m_device;
	
	void _build(const VKLImageCreateInfo& createInfo);
};

#endif
