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

const VkPipelineVertexInputStateCreateInfo* VKLShader::getVertexInputState() const {
	return &m_vertexInputState;
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
	
	m_vertexInputBindingDescs = (VkVertexInputBindingDescription*)malloc(sizeof(VkVertexInputBindingDescription) *
																	   createInfo.vertexInputBindings.size());
	for(int i = 0; i < createInfo.vertexInputBindings.size(); i++) {
		m_vertexInputBindingDescs[i] = createInfo.vertexInputBindings[i].desc;
	}
	
	m_vertexAttribDescs = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription) *
																	 createInfo.vertexAttribs.size());
	memcpy(m_vertexAttribDescs, createInfo.vertexAttribs.data(), sizeof(VkVertexInputAttributeDescription) *
																	createInfo.vertexAttribs.size());
	
	m_vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertexInputState.pNext = NULL;
	m_vertexInputState.flags = 0;
	m_vertexInputState.pVertexAttributeDescriptions = m_vertexAttribDescs;
	m_vertexInputState.vertexAttributeDescriptionCount = createInfo.vertexAttribs.size();
	m_vertexInputState.pVertexBindingDescriptions = m_vertexInputBindingDescs;
	m_vertexInputState.vertexBindingDescriptionCount = createInfo.vertexInputBindings.size();
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

VKLVertexInputBinding& VKLShaderCreateInfo::addVertexInputBinding(uint32_t binding) {
	vertexInputBindings.push_back(VKLVertexInputBinding(binding, *this));
	
	return vertexInputBindings.back();
}

void VKLShaderCreateInfo::addVertexAttrib(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) {
	VkVertexInputAttributeDescription attribDesc;
	
	attribDesc.location = location;
	attribDesc.binding = binding;
	attribDesc.format = format;
	attribDesc.offset = offset;
	
	vertexAttribs.push_back(attribDesc);
}

bool VKLShaderCreateInfo::validate() {
	if(device == NULL || shaderModuleCreateInfos.size() == 0) {
		return false;
	}
	
	return true;
}

VKLVertexInputBinding::VKLVertexInputBinding(uint32_t binding, VKLShaderCreateInfo& parent) : parent(parent) {
	memset(&desc, 0, sizeof(VkVertexInputBindingDescription));
	desc.binding = binding;
	desc.stride = 0;
	desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

VKLVertexInputBinding& VKLVertexInputBinding::setStride(uint32_t stride) {
	desc.stride = stride;
	
	return *this;
}

VKLVertexInputBinding& VKLVertexInputBinding::setInputRate(VkVertexInputRate inputRate) {
	desc.inputRate = inputRate;
	
	return *this;
}

VKLVertexInputBinding& VKLVertexInputBinding::addAttrib(uint32_t location, VkFormat format, uint32_t offset) {
	parent.addVertexAttrib(location, desc.binding, format, offset);
	
	return *this;
}

VKLShaderCreateInfo& VKLVertexInputBinding::end() {
	return parent;
}
