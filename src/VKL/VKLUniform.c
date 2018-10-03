#include <VKL/VKL.h>

int vklCreateUniformObject(VKLDevice* device, VKLUniformObject** pUniform, VKLShader* shader) {
	VKLUniformObject* uniform = malloc_c(sizeof(VKLUniformObject));

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
	memset(&descriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = shader->descriptorPoolSizesCount;
	descriptorPoolCreateInfo.pPoolSizes = shader->descriptorPoolSizes;

	VLKCheck(device->pvkCreateDescriptorPool(device->device, &descriptorPoolCreateInfo, NULL, &uniform->descriptorPool),
		"Failed to create descriptor pool");

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	memset(&descriptorSetAllocateInfo, 0, sizeof(VkDescriptorSetAllocateInfo));
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = uniform->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &shader->descriptorSetLayout;

	VLKCheck(device->pvkAllocateDescriptorSets(device->device, &descriptorSetAllocateInfo, &uniform->descriptorSet),
		"Failed to allocate descriptor sets");

	*pUniform = uniform;

	return 0;
}

int vklSetUniformBuffer(VKLDevice* device, VKLUniformObject* uniform, VKLBuffer* buffer, uint32_t index) {
	VkDescriptorBufferInfo descriptorBufferInfo;
	memset(&descriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
	descriptorBufferInfo.buffer = buffer->buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet writeDescriptor;
	memset(&writeDescriptor, 0, sizeof(VkWriteDescriptorSet));
	writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptor.dstSet = uniform->descriptorSet;
	writeDescriptor.dstBinding = index;
	writeDescriptor.dstArrayElement = 0;
	writeDescriptor.descriptorCount = 1;
	writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptor.pImageInfo = NULL;
	writeDescriptor.pBufferInfo = &descriptorBufferInfo;
	writeDescriptor.pTexelBufferView = NULL;

	device->pvkUpdateDescriptorSets(device->device, 1, &writeDescriptor, 0, NULL);
	
	return 0;
}

int vklSetUniformStorageBuffer(VKLDevice* device, VKLUniformObject* uniform, VKLBuffer* buffer, uint32_t index) {
	VkDescriptorBufferInfo descriptorBufferInfo;
	memset(&descriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
	descriptorBufferInfo.buffer = buffer->buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet writeDescriptor;
	memset(&writeDescriptor, 0, sizeof(VkWriteDescriptorSet));
	writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptor.dstSet = uniform->descriptorSet;
	writeDescriptor.dstBinding = index;
	writeDescriptor.dstArrayElement = 0;
	writeDescriptor.descriptorCount = 1;
	writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	writeDescriptor.pImageInfo = NULL;
	writeDescriptor.pBufferInfo = &descriptorBufferInfo;
	writeDescriptor.pTexelBufferView = NULL;

	device->pvkUpdateDescriptorSets(device->device, 1, &writeDescriptor, 0, NULL);

	return 0;
}

int vklSetUniformTexture(VKLDevice* device, VKLUniformObject* uniform, VKLTexture* texture, uint32_t index) {
	VkDescriptorImageInfo descriptorImageInfo;
	memset(&descriptorImageInfo, 0, sizeof(VkDescriptorImageInfo));
	descriptorImageInfo.sampler = texture->sampler;
	descriptorImageInfo.imageView = texture->imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet writeDescriptor;
	memset(&writeDescriptor, 0, sizeof(VkWriteDescriptorSet));
	writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptor.dstSet = uniform->descriptorSet;
	writeDescriptor.dstBinding = index;
	writeDescriptor.dstArrayElement = 0;
	writeDescriptor.descriptorCount = 1;
	writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptor.pImageInfo = &descriptorImageInfo;
	writeDescriptor.pBufferInfo = NULL;
	writeDescriptor.pTexelBufferView = NULL;

	device->pvkUpdateDescriptorSets(device->device, 1, &writeDescriptor, 0, NULL);
	return 0;
}

int vklDestroyUniformObject(VKLDevice* device, VKLUniformObject* uniform) {
	device->pvkDestroyDescriptorPool(device->device, uniform->descriptorPool, device->instance->allocator);

	free_c(uniform);
	return 0;
}
