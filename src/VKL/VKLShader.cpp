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

const VkDescriptorSetLayout* VKLShader::getDescriptorSetLayouts() const {
	return m_descriptorSetLayouts;
}

void VKLShader::destroy() {
	for(int i = 0; i < m_shaderStageCreateInfos.size(); i++) {
		m_device->vk.DestroyShaderModule(m_device->handle(), m_shaderStageCreateInfos[i].module, m_device->allocationCallbacks());
	}
	
	for(int i = 0; i < m_descSetCount; i++) {
		m_device->vk.DestroyDescriptorSetLayout(m_device->handle(), m_descriptorSetLayouts[i], m_device->allocationCallbacks());
	}
	
	m_device->vk.DestroyPipelineLayout(m_device->handle(), m_layout, m_device->allocationCallbacks());
	
	free(m_pushConstantRanges);
	free(m_vertexAttribDescs);
	free(m_vertexInputBindingDescs);
	free(m_descriptorSetLayouts);
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
	
	m_descriptorSetLayouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout)
															* createInfo.descriptorSetLayouts.size());
	
	m_descSetCount = createInfo.descriptorSetLayouts.size();
	
	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo;
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.pNext = NULL;
	descSetLayoutInfo.flags = 0;
	for(int i = 0; i < createInfo.descriptorSetLayouts.size(); i++) {
		descSetLayoutInfo.bindingCount = createInfo.descriptorSetLayouts[i].bindings.size();
		descSetLayoutInfo.pBindings = createInfo.descriptorSetLayouts[i].bindings.data();
		
		VK_CALL(m_device->vk.CreateDescriptorSetLayout(m_device->handle(), &descSetLayoutInfo,
													   m_device->allocationCallbacks(), &m_descriptorSetLayouts[i]));
	}
	
	m_pushConstantRanges = (VkPushConstantRange*)malloc(sizeof(VkPushConstantRange) * createInfo.pushConstantRanges.size());
	memcpy(m_pushConstantRanges, createInfo.pushConstantRanges.data(),
		   sizeof(VkPushConstantRange) * createInfo.pushConstantRanges.size());
	
	VkPipelineLayoutCreateInfo layoutCreateInfo;
	memset(&layoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = createInfo.descriptorSetLayouts.size();
	layoutCreateInfo.pSetLayouts = m_descriptorSetLayouts;
	layoutCreateInfo.pushConstantRangeCount = createInfo.pushConstantRanges.size();
	layoutCreateInfo.pPushConstantRanges = m_pushConstantRanges;
	
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


VKLDescriptorSetLayoutCreateInfo& VKLShaderCreateInfo::addDescriptorSet() {
	descriptorSetLayouts.push_back(VKLDescriptorSetLayoutCreateInfo(*this));
	
	return descriptorSetLayouts.back();
}

VKLShaderCreateInfo& VKLShaderCreateInfo::addPushConstant(VkShaderStageFlags stage, uint32_t offset, uint32_t size) {
	VkPushConstantRange result;
	result.stageFlags = stage;
	result.offset = offset;
	result.size = size;
	
	pushConstantRanges.push_back(result);
	
	return *this;
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

VKLDescriptorSetLayoutCreateInfo::VKLDescriptorSetLayoutCreateInfo(VKLShaderCreateInfo& parent) : parent(parent){
	
}

VKLDescriptorSetLayoutCreateInfo& VKLDescriptorSetLayoutCreateInfo::addBinding(uint32_t binding, VkDescriptorType type,
																			   uint32_t count, VkShaderStageFlags stage) {
	VkDescriptorSetLayoutBinding result;
	result.binding = binding;
	result.descriptorType = type;
	result.descriptorCount = count;
	result.stageFlags = stage;
	result.pImmutableSamplers = NULL;
	
	bindings.push_back(result);
	
	return *this;
}

VKLShaderCreateInfo& VKLDescriptorSetLayoutCreateInfo::end() {
	return parent;
}
