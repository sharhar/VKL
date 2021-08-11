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

class VKLVertexInputBinding {
public:
	VKLVertexInputBinding(uint32_t binding, VKLShaderCreateInfo& parent);
	
	VKLVertexInputBinding& setStride(uint32_t stride);
	VKLVertexInputBinding& setInputRate(VkVertexInputRate inputRate);
	VKLVertexInputBinding& addAttrib(uint32_t location, VkFormat format, uint32_t offset);
	
	VKLShaderCreateInfo& end();
	
	VkVertexInputBindingDescription desc;
	
	VKLShaderCreateInfo& parent;
};

/*
 
 bindings[0].binding = 0;
 bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
 bindings[0].descriptorCount = 1;
 bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
 bindings[0].pImmutableSamplers = NULL;
 */

class VKLDescriptorSetLayoutCreateInfo {
public:
	VKLDescriptorSetLayoutCreateInfo(VKLShaderCreateInfo& parent);
	
	VKLDescriptorSetLayoutCreateInfo& addBinding(uint32_t binding, VkDescriptorType type,
												uint32_t count, VkShaderStageFlags stage);
	VKLShaderCreateInfo& end();
	
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	VKLShaderCreateInfo& parent;
};

class VKLShaderCreateInfo : public VKLCreateInfo<VKLShaderCreateInfo>{
public:
	VKLShaderCreateInfo();
	
	VKLShaderCreateInfo& setDevice(const VKLDevice* device);
	VKLVertexInputBinding& addVertexInputBinding(uint32_t binding);
	VKLDescriptorSetLayoutCreateInfo& addDescriptorSet();
	VKLShaderCreateInfo& addPushConstant(VkShaderStageFlags stage, uint32_t offset, uint32_t size);
	VKLShaderCreateInfo& addShaderModule(const uint32_t* pCode, size_t codeSize,
										 VkShaderStageFlagBits stage, const char* entryPoint);
	
	std::vector<VKLShaderModuleCreateInfo> shaderModuleCreateInfos;
	std::vector<VKLVertexInputBinding> vertexInputBindings;
	std::vector<VkVertexInputAttributeDescription> vertexAttribs;
	std::vector<VkPushConstantRange> pushConstantRanges;
	std::vector<VKLDescriptorSetLayoutCreateInfo> descriptorSetLayouts;
	const VKLDevice* device;
	
private:
	friend VKLVertexInputBinding& VKLVertexInputBinding::addAttrib(uint32_t location, VkFormat format, uint32_t offset);
	
	void addVertexAttrib(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

	bool _validate();
};

class VKLShader : public VKLCreator<VKLShaderCreateInfo>{
public:
	VKLShader();
	VKLShader(const VKLShaderCreateInfo& createInfo);
	
	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStageCreateInfos() const;
	VkPipelineLayout getPipelineLayout() const;
	const VKLDevice* device() const;
	const VkPipelineVertexInputStateCreateInfo* getVertexInputState() const;
	const VkDescriptorSetLayout* getDescriptorSetLayouts() const;
private:
	const VKLDevice* m_device;
	
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
	
	uint32_t m_descSetCount;
	
	VkPipelineLayout m_layout;
	VkVertexInputBindingDescription* m_vertexInputBindingDescs;
	VkVertexInputAttributeDescription* m_vertexAttribDescs;
	VkPushConstantRange* m_pushConstantRanges;
	VkDescriptorSetLayout* m_descriptorSetLayouts;
	VkPipelineVertexInputStateCreateInfo m_vertexInputState;
	
	void _destroy();
	void _create(const VKLShaderCreateInfo& createInfo);
};

#endif /* VKLShader_h */
