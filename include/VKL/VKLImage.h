#ifndef VKLImage_h
#define VKLImage_h

#include "VKL_base.h"

typedef struct VKLImageCreateInfo {
	VKLImageCreateInfo(VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage);

	VkFormat format;
	uint32_t width, height, depth;
	VkImageType imageType;
	uint32_t mipLevels, arrayLayers;
	VkSampleCountFlags sampleCount;
	VkImageTiling tiling;
	VkImageUsageFlags usage;
	VkImageLayout initialLayout;

	VkImage imgHandle;
} VKLImageCreateInfo;

class VKLImage {
public:
	VKLImage();
	VKLImage(VKLImageCreateInfo* createInfo, VKLDevice* device);

	void create(VKLImageCreateInfo* createInfo, VKLDevice* device);
	
	void setNewAccessMask(VkAccessFlags accessMask);
	void setNewLayout(VkImageLayout layout);
	
	VkImageMemoryBarrier* getMemoryBarrier();
	void resetBarrier();

	VkImage handle();
	VkImageView view();
	void destroy();
private:
	VkImage m_handle;
	VkImageView m_view;
	
	VkImageMemoryBarrier m_memoryBarrier;

	VmaAllocation m_allocation;

	VKLDevice* m_device;
};

#endif
