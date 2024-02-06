#ifndef VKLImage_h
#define VKLImage_h

#include "VKL_base.h"

class VKLImageCreateInfo : public VKLCreateInfo<VKLImageCreateInfo>  {
public:
	VKLImageCreateInfo();
	
	VKLImageCreateInfo& device(const VKLDevice* device);

	VKLImageCreateInfo& pNext(void* pNext);

	VKLImageCreateInfo& allocationPNext(void* pNext);
	
	VKLImageCreateInfo& imageType(VkImageType type);
	VKLImageCreateInfo& format(VkFormat format);
	VKLImageCreateInfo& extent(uint32_t width, uint32_t height, uint32_t depth);
	VKLImageCreateInfo& tiling(VkImageTiling tiling);
	VKLImageCreateInfo& usage(VkImageUsageFlags usage);
	VKLImageCreateInfo& initialLayout(VkImageLayout layout);
	
	VKLImageCreateInfo& memoryProperties(VkMemoryPropertyFlags memoryProperties);	

private:
	VkImageCreateInfo m_imageCreateInfo;

	void* m_allocationPNext;
	
	VkMemoryPropertyFlags m_memoryProperties;
	
	const VKLDevice* m_device;

	bool _validate();

	friend class VKLImage;
};

class VKLImage : public VKLHandle<VkImage>, public VKLCreator<VKLImageCreateInfo> {
public:
	VKLImage();
	VKLImage(const VKLImageCreateInfo& createInfo);
	
	VkAccessFlags accessMask() const;
	VkImageLayout layout() const;
	
	VkExtent3D extent() const;
	VkImageAspectFlags aspect() const;

	VkDeviceMemory memory() const;

	VkMemoryRequirements memoryRequirements() const;
	
	void transition(const VKLQueue* queue, VkAccessFlags accessMask, VkImageLayout layout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
	void cmdTransitionBarrier(VKLCommandBuffer* cmdBuffer, VkAccessFlags accessMask, VkImageLayout layout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

	void copyFrom(VKLImage* src, const VKLQueue* transferQueue, VkImageCopy imageCopy);
	void setData(void* data, size_t size, size_t pixelSize);
	void getData(void* data, size_t size, size_t pixelSize);
	void uploadData(const VKLQueue* transferQueue, void* data, size_t size, size_t pixelSize);

	VkFormat format() const;
	
	VkImageAspectFlags m_aspect;
private:
	VkImageMemoryBarrier m_memoryBarrier;
	VkDeviceMemory m_memory;

	VkFormat m_format;
	
	VkExtent3D m_size;

	void initBarrier(VkImageLayout layout);

	const VKLDevice* m_device;
	
	void _destroy();
	void _create(const VKLImageCreateInfo& createInfo);
	void _create(const VKLImageCreateInfo& createInfo, VkImage handle);

	friend class VKLImageView;
	friend class VKLSwapChain;
};

#endif
