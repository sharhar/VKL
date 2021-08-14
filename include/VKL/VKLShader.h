//
//  VKLShader.h
//  VKL
//
//  Created by Shahar Sandhaus on 8/6/21.
//

#ifndef VKLShader_h
#define VKLShader_h

#include <VKL/VKL_base.h>

typedef struct {
	VkShaderModuleCreateInfo createInfo;
	VkShaderStageFlagBits stage;
	const char* entryPoint;
} VKLShaderModuleCreateInfo;

class VKLShaderCreateInfo;

class VKLDescriptorSetLayoutCreateInfo {
public:
	VKLDescriptorSetLayoutCreateInfo& addBinding(uint32_t binding, VkDescriptorType type,
												uint32_t count, VkShaderStageFlags stage);
	VKLShaderCreateInfo& end();
	
private:
	VKLDescriptorSetLayoutCreateInfo(VKLShaderCreateInfo& parent);

	std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	VKLShaderCreateInfo& m_parent;

	friend class VKLShaderCreateInfo;
	friend class VKLShader;
};

class VKLShaderCreateInfo : public VKLCreateInfo<VKLShaderCreateInfo>{
public:
	VKLShaderCreateInfo();
	
	VKLShaderCreateInfo& device(const VKLDevice* device);
	VKLDescriptorSetLayoutCreateInfo& addDescriptorSet();
	VKLShaderCreateInfo& addPushConstant(VkShaderStageFlags stage, uint32_t offset, uint32_t size);
	VKLShaderCreateInfo& addShaderModule(const uint32_t* pCode, size_t codeSize,
										 VkShaderStageFlagBits stage, const char* entryPoint);
private:

	std::vector<VKLShaderModuleCreateInfo> m_shaderModuleCreateInfos;
	std::vector<VkVertexInputAttributeDescription> m_vertexAttribs;
	std::vector<VkPushConstantRange> m_pushConstantRanges;
	std::vector<VKLDescriptorSetLayoutCreateInfo> m_descriptorSetLayouts;
	const VKLDevice* m_device;
	
	void addVertexAttrib(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

	bool _validate();

	friend class VKLDescriptorSetLayoutCreateInfo;
	friend class VKLShader;
};

class VKLShader : public VKLCreator<VKLShaderCreateInfo>{
public:
	VKLShader();
	VKLShader(const VKLShaderCreateInfo& createInfo);
	
	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStageCreateInfos() const;
	VkPipelineLayout getPipelineLayout() const;
	const VKLDevice* device() const;
	const VkDescriptorSetLayout* getDescriptorSetLayouts() const;
private:
	const VKLDevice* m_device;
	
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
	
	uint32_t m_descSetCount;
	
	VkPipelineLayout m_layout;
	//VkVertexInputBindingDescription* m_vertexInputBindingDescs;
	//VkVertexInputAttributeDescription* m_vertexAttribDescs;
	VkPushConstantRange* m_pushConstantRanges;
	VkDescriptorSetLayout* m_descriptorSetLayouts;
	
	void _destroy();
	void _create(const VKLShaderCreateInfo& createInfo);
};

#endif /* VKLShader_h */
