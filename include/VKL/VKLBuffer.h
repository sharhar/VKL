//
//  VKLBuffer.h
//  VKL
//
//  Created by Shahar Sandhaus on 7/22/21.
//

#ifndef VKLBuffer_h
#define VKLBuffer_h

#include "VKL_base.h"

class VKLBuffer : public VKLObject<VkBuffer> {
public:
	VKLBuffer();
	
	VKLBuffer& setDevice(VKLDevice* device);
	VKLBuffer& ciSetSize(VkDeviceSize size);
	VKLBuffer& ciAddUsage(VkBufferUsageFlags usage);
	VKLBuffer& ciAddAllocationFlag(VmaAllocationCreateFlags flags);
	VKLBuffer& ciSetMemoryUsage(VmaMemoryUsage memoryUsage);
	
	void setData(void* data, size_t size, size_t offset);
	
	void destroy();
	
private:
	VKLDevice* m_device;
	
	VmaAllocation m_allocation;
	
	VkBufferCreateInfo m_bufferCreateInfo;
	VmaAllocationCreateInfo m_allocationCreateInfo;
	
	void _build();
};

#endif /* VKLBuffer_h */
