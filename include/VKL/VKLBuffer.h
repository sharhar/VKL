//
//  VKLBuffer.h
//  VKL
//
//  Created by Shahar Sandhaus on 7/22/21.
//

#ifndef VKLBuffer_h
#define VKLBuffer_h

#include "VKL_base.h"

class VKLBufferCreateInfo : public VKLCreateInfo<VKLBufferCreateInfo> {
public:
	VKLBufferCreateInfo();
	
	VKLBufferCreateInfo& setDevice(const VKLDevice* device);
	VKLBufferCreateInfo& setSize(VkDeviceSize size);
	VKLBufferCreateInfo& setUsage(VkBufferUsageFlags usage);
	VKLBufferCreateInfo& setAllocationFlags(VmaAllocationCreateFlags flags);
	VKLBufferCreateInfo& setMemoryUsage(VmaMemoryUsage memoryUsage);
	
	VkBufferCreateInfo bufferCreateInfo;
	VmaAllocationCreateInfo allocationCreateInfo;
	
	const VKLDevice* device;
	
private:
	bool _validate();
};

class VKLBuffer : public VKLHandle<VkBuffer>, public VKLCreator<VKLBufferCreateInfo> {
public:
	VKLBuffer();
	VKLBuffer(const VKLBufferCreateInfo& createInfo);
	
	void setData(void* data, size_t size, size_t offset);
	
	void setNewAccessMask(VkAccessFlags accessMask);
	VkBufferMemoryBarrier* getMemoryBarrier();
	void resetBarrier();
	
	void copyFrom(VKLBuffer* src, const VKLQueue* transferQueue, VkBufferCopy bufferCopy);
	void uploadData(const VKLQueue* transferQueue, void* data, size_t size, size_t offset);
private:
	const VKLDevice* m_device;
	
	VmaAllocation m_allocation;
	
	VkBufferMemoryBarrier m_memoryBarrier;
	
	void _destroy();
	void _create(const VKLBufferCreateInfo& createInfo);
};

#endif /* VKLBuffer_h */
