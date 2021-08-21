#ifndef VKLDescriptorSet_h
#define VKLDescriptorSet_h

#include <VKL/VKL_base.h>

class VKLDescriptorSet : public VKLHandle<VkDescriptorSet>{
public:
	VKLDescriptorSet(const VKLShader* shader, uint32_t set);
	
	VkDescriptorPool pool() const;
	
	void writeImage(uint32_t binding, VkDescriptorType type, VkImageView view, VkImageLayout layout, VkSampler sampler);
	
	void destroy();
private:
	const VKLShader* m_shader;
	const VKLDevice* m_device;
	
	VkDescriptorPool m_pool;
};

#endif /* VKLDescriptorSet_h */
