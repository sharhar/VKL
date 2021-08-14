#ifndef VKLImage_h
#define VKLImage_h

#include "VKL_base.h"

class VKLImageCreateInfo : public VKLCreateInfo<VKLImageCreateInfo>  {
public:
	VKLImageCreateInfo();
	
	VKLImageCreateInfo& device(const VKLDevice* device);
	
	VKLImageCreateInfo& imageType(VkImageType type);
	VKLImageCreateInfo& format(VkFormat format);
	VKLImageCreateInfo& extent(uint32_t width, uint32_t height, uint32_t depth);
	VKLImageCreateInfo& tiling(VkImageTiling tiling);
	VKLImageCreateInfo& usage(VkImageUsageFlags usage);
	VKLImageCreateInfo& initialLayout(VkImageLayout layout);
	
	VKLImageCreateInfo& allocationFlags(VmaAllocationCreateFlags flags);
	VKLImageCreateInfo& memoryUsage(VmaMemoryUsage memoryUsage);

private:
	VkImageCreateInfo m_imageCreateInfo;
	VmaAllocationCreateInfo m_allocationCreateInfo;
	
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

	VkFormat format() const;
private:
	VkImageMemoryBarrier m_memoryBarrier;
	VmaAllocation m_allocation;

	VkFormat m_format;

	void initBarrier(VkImageLayout layout);

	const VKLDevice* m_device;
	
	void _destroy();
	void _create(const VKLImageCreateInfo& createInfo);

	friend class VKLImageView;
	friend class VKLSwapChain;
};

#endif
