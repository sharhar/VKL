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

class VKLShaderCreateInfo : public VKLCreateInfo{
public:
	VKLShaderCreateInfo();
	
	VKLShaderCreateInfo& setDevice(const VKLDevice* device);
	VKLShaderCreateInfo& addShaderModule(const uint32_t* pCode, size_t codeSize, VkShaderStageFlagBits stage, const char* entryPoint);
	VKLVertexInputBinding& addVertexInputBinding(uint32_t binding);
	
	bool validate();
	
	std::vector<VKLShaderModuleCreateInfo> shaderModuleCreateInfos;
	std::vector<VKLVertexInputBinding> vertexInputBindings;
	std::vector<VkVertexInputAttributeDescription> vertexAttribs;
	const VKLDevice* device;
	
private:
	friend VKLVertexInputBinding& VKLVertexInputBinding::addAttrib(uint32_t location, VkFormat format, uint32_t offset);
	
	void addVertexAttrib(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);
};

class VKLShader : public VKLBuilder<VKLShaderCreateInfo>{
public:
	VKLShader();
	VKLShader(const VKLShaderCreateInfo& createInfo);
	
	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStageCreateInfos() const;
	VkPipelineLayout getPipelineLayout() const;
	const VKLDevice* device() const;
	const VkPipelineVertexInputStateCreateInfo* getVertexInputState() const;
	
	void destroy();
private:
	const VKLDevice* m_device;
	
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
	
	VkPipelineLayout m_layout;
	VkVertexInputBindingDescription* m_vertexInputBindingDescs;
	VkVertexInputAttributeDescription* m_vertexAttribDescs;
	VkPipelineVertexInputStateCreateInfo m_vertexInputState;
	
	void _build(const VKLShaderCreateInfo& createInfo);
};

#endif /* VKLShader_h */
