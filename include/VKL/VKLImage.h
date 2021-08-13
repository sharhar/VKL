#ifndef VKLImage_h
#define VKLImage_h

#include "VKL_base.h"

class VKLImageCreateInfo : public VKLCreateInfo<VKLImageCreateInfo>  {
public:
	VKLImageCreateInfo();
	
	VKLImageCreateInfo& device(const VKLDevice* device);
	
	VKLImageCreateInfo& handle(VkImage image);
	
	VKLImageCreateInfo& imageType(VkImageType type);
	VKLImageCreateInfo& format(VkFormat format);
	VKLImageCreateInfo& extent(uint32_t width, uint32_t height, uint32_t depth);
	VKLImageCreateInfo& tiling(VkImageTiling tiling);
	VKLImageCreateInfo& usage(VkImageUsageFlags usage);
	VKLImageCreateInfo& initialLayout(VkImageLayout layout);
	
	VKLImageCreateInfo& viewType(VkImageViewType type);
	VKLImageCreateInfo& viewFormat(VkFormat format);
	
	VKLImageCreateInfo& allocationFlags(VmaAllocationCreateFlags flags);
	VKLImageCreateInfo& memoryUsage(VmaMemoryUsage memoryUsage);

private:
	VkImageCreateInfo m_imageCreateInfo;
	VkImageViewCreateInfo m_viewCreateInfo;
	VmaAllocationCreateInfo m_allocationCreateInfo;
	VkImage m_handle;
	
	const VKLDevice* m_device;

	bool _validate();

	friend class VKLImage;
};

class VKLImage : public VKLHandle<VkImage>, public VKLCreator<VKLImageCreateInfo> {
public:
	VKLImage();
	VKLImage(const VKLImageCreateInfo& createInfo);
	
	void setNewAccessMask(VkAccessFlags accessMask);
	void setNewLayout(VkImageLayout layout);
	
	VkImageMemoryBarrier* getMemoryBarrier();
	void resetBarrier();

	VkImageView view();
private:
	VkImageView m_view;
	VkImageMemoryBarrier m_memoryBarrier;
	VmaAllocation m_allocation;

	const VKLDevice* m_device;
	
	void _destroy();
	void _create(const VKLImageCreateInfo& createInfo);
};

#endif
