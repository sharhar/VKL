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

class VKLShaderCreateInfo : public VKLCreateInfo{
public:
	VKLShaderCreateInfo();
	
	VKLShaderCreateInfo& setDevice(const VKLDevice* device);
	VKLShaderCreateInfo& addShaderModule(const uint32_t* pCode, size_t codeSize, VkShaderStageFlagBits stage, const char* entryPoint);
	
	bool validate();
	
	std::vector<VKLShaderModuleCreateInfo> shaderModuleCreateInfos;
	const VKLDevice* device;
};

class VKLShader : public VKLBuilder<VKLShaderCreateInfo>{
public:
	VKLShader();
	VKLShader(const VKLShaderCreateInfo& createInfo);
	
	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStageCreateInfos() const;
	VkPipelineLayout getPipelineLayout() const;
	const VKLDevice* device() const;
	
	void destroy();
private:
	const VKLDevice* m_device;
	
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
	
	VkPipelineLayout m_layout;
	
	void _build(const VKLShaderCreateInfo& createInfo);
};

#endif /* VKLShader_h */
