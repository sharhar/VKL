#include <VKL/VKL.h>

VKLShader::VKLShader() : VKLBuilder<VKLShaderCreateInfo>("VKLShader") {
	
}

VKLShader::VKLShader(const VKLShaderCreateInfo& createInfo) : VKLBuilder<VKLShaderCreateInfo>("VKLShader") {
	
	this->build(createInfo);
}

const std::vector<VkPipelineShaderStageCreateInfo>& VKLShader::getShaderStageCreateInfos() const {
	return m_shaderStageCreateInfos;
}

VkPipelineLayout VKLShader::getPipelineLayout() const {
	return m_layout;
}

const VKLDevice* VKLShader::device() const {
	return m_device;
}

void VKLShader::destroy() {
	for(int i = 0; i < m_shaderStageCreateInfos.size(); i++) {
		m_device->vk.DestroyShaderModule(m_device->handle(), m_shaderStageCreateInfos[i].module, m_device->allocationCallbacks());
	}
	
	m_device->vk.DestroyPipelineLayout(m_device->handle(), m_layout, m_device->allocationCallbacks());
}

void VKLShader::_build(const VKLShaderCreateInfo& createInfo) {
	m_device = createInfo.device;
	
	m_shaderStageCreateInfos.resize(createInfo.shaderModuleCreateInfos.size());
	
	for(int i = 0; i < m_shaderStageCreateInfos.size(); i++) {
		m_shaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_shaderStageCreateInfos[i].pNext = NULL;
		m_shaderStageCreateInfos[i].flags = 0;
		m_shaderStageCreateInfos[i].pName = createInfo.shaderModuleCreateInfos[i].entryPoint;
		m_shaderStageCreateInfos[i].stage = createInfo.shaderModuleCreateInfos[i].stage;
		
		VK_CALL(m_device->vk.CreateShaderModule(m_device->handle(),
												&createInfo.shaderModuleCreateInfos[i].createInfo,
												m_device->allocationCallbacks(), &m_shaderStageCreateInfos[i].module));
	}
	
	VkPipelineLayoutCreateInfo layoutCreateInfo;
	memset(&layoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 0;
	layoutCreateInfo.pSetLayouts = NULL;
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = NULL;
	
	VK_CALL(m_device->vk.CreatePipelineLayout(m_device->handle(), &layoutCreateInfo, m_device->allocationCallbacks(), &m_layout));
}

VKLShaderCreateInfo::VKLShaderCreateInfo() {
	this->device = NULL;
}

VKLShaderCreateInfo& VKLShaderCreateInfo::setDevice(const VKLDevice* device) {
	this->device = device;
	
	return *this;
}

VKLShaderCreateInfo& VKLShaderCreateInfo::addShaderModule(const uint32_t* pCode, size_t codeSize,
														VkShaderStageFlagBits stage, const char* entryPoint) {
	VKLShaderModuleCreateInfo result;
	
	result.createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	result.createInfo.pNext = NULL;
	result.createInfo.flags = 0;
	result.createInfo.pCode = pCode;
	result.createInfo.codeSize = codeSize;
	
	result.stage = stage;
	result.entryPoint = entryPoint;
	
	shaderModuleCreateInfos.push_back(result);
	
	return *this;
	
}

bool VKLShaderCreateInfo::validate() {
	if(device == NULL || shaderModuleCreateInfos.size() == 0) {
		return false;
	}
	
	return true;
}
