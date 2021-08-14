#include <VKL/VKL.h>

VKLShader::VKLShader() : VKLCreator<VKLShaderCreateInfo>("VKLShader") {
	
}

VKLShader::VKLShader(const VKLShaderCreateInfo& createInfo) : VKLCreator<VKLShaderCreateInfo>("VKLShader") {
	this->create(createInfo);
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

const VkDescriptorSetLayout* VKLShader::getDescriptorSetLayouts() const {
	return m_descriptorSetLayouts;
}

void VKLShader::_destroy() {
	for(int i = 0; i < m_shaderStageCreateInfos.size(); i++) {
		m_device->vk.DestroyShaderModule(m_device->handle(), m_shaderStageCreateInfos[i].module, m_device->allocationCallbacks());
	}
	
	for(int i = 0; i < m_descSetCount; i++) {
		m_device->vk.DestroyDescriptorSetLayout(m_device->handle(), m_descriptorSetLayouts[i], m_device->allocationCallbacks());
	}
	
	m_device->vk.DestroyPipelineLayout(m_device->handle(), m_layout, m_device->allocationCallbacks());
	
	free(m_pushConstantRanges);
	free(m_descriptorSetLayouts);
}

void VKLShader::_create(const VKLShaderCreateInfo& createInfo) {
	m_device = createInfo.m_device;
	
	m_shaderStageCreateInfos.resize(createInfo.m_shaderModuleCreateInfos.size());
	
	for(int i = 0; i < m_shaderStageCreateInfos.size(); i++) {
		m_shaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_shaderStageCreateInfos[i].pNext = NULL;
		m_shaderStageCreateInfos[i].flags = 0;
		m_shaderStageCreateInfos[i].pName = createInfo.m_shaderModuleCreateInfos[i].entryPoint;
		m_shaderStageCreateInfos[i].stage = createInfo.m_shaderModuleCreateInfos[i].stage;
		
		VK_CALL(m_device->vk.CreateShaderModule(m_device->handle(),
												&createInfo.m_shaderModuleCreateInfos[i].createInfo,
												m_device->allocationCallbacks(), &m_shaderStageCreateInfos[i].module));
	}
	
	m_descriptorSetLayouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout)
															* createInfo.m_descriptorSetLayouts.size());
	
	m_descSetCount = createInfo.m_descriptorSetLayouts.size();
	
	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo;
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.pNext = NULL;
	descSetLayoutInfo.flags = 0;
	for(int i = 0; i < createInfo.m_descriptorSetLayouts.size(); i++) {
		descSetLayoutInfo.bindingCount = createInfo.m_descriptorSetLayouts[i].m_bindings.size();
		descSetLayoutInfo.pBindings = createInfo.m_descriptorSetLayouts[i].m_bindings.data();
		
		VK_CALL(m_device->vk.CreateDescriptorSetLayout(m_device->handle(), &descSetLayoutInfo,
													   m_device->allocationCallbacks(), &m_descriptorSetLayouts[i]));
	}
	
	m_pushConstantRanges = (VkPushConstantRange*)malloc(sizeof(VkPushConstantRange) * createInfo.m_pushConstantRanges.size());
	memcpy(m_pushConstantRanges, createInfo.m_pushConstantRanges.data(),
		   sizeof(VkPushConstantRange) * createInfo.m_pushConstantRanges.size());
	
	VkPipelineLayoutCreateInfo layoutCreateInfo;
	memset(&layoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = createInfo.m_descriptorSetLayouts.size();
	layoutCreateInfo.pSetLayouts = m_descriptorSetLayouts;
	layoutCreateInfo.pushConstantRangeCount = createInfo.m_pushConstantRanges.size();
	layoutCreateInfo.pPushConstantRanges = m_pushConstantRanges;
	
	VK_CALL(m_device->vk.CreatePipelineLayout(m_device->handle(), &layoutCreateInfo, m_device->allocationCallbacks(), &m_layout));
}

VKLShaderCreateInfo::VKLShaderCreateInfo() {
	m_device = NULL;
}

VKLShaderCreateInfo& VKLShaderCreateInfo::device(const VKLDevice* device) {
	m_device = device;
	
	return invalidate();
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
	
	m_shaderModuleCreateInfos.push_back(result);
	
	return invalidate();
	
}

void VKLShaderCreateInfo::addVertexAttrib(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) {
	VkVertexInputAttributeDescription attribDesc;
	
	attribDesc.location = location;
	attribDesc.binding = binding;
	attribDesc.format = format;
	attribDesc.offset = offset;
	
	m_vertexAttribs.push_back(attribDesc);

	invalidate();
}


VKLDescriptorSetLayoutCreateInfo& VKLShaderCreateInfo::addDescriptorSet() {
	m_descriptorSetLayouts.push_back(VKLDescriptorSetLayoutCreateInfo(*this));

	invalidate();
	
	return m_descriptorSetLayouts.back();
}

VKLShaderCreateInfo& VKLShaderCreateInfo::addPushConstant(VkShaderStageFlags stage, uint32_t offset, uint32_t size) {
	VkPushConstantRange result;
	result.stageFlags = stage;
	result.offset = offset;
	result.size = size;
	
	m_pushConstantRanges.push_back(result);
	
	return invalidate();
}

bool VKLShaderCreateInfo::_validate() {
	if(m_device == NULL) {
		printf("VKL Validation Error: VKLShaderCreateInfo::device is not set!\n");
		return false;
	}

	if (m_shaderModuleCreateInfos.size() == 0) {
		printf("VKL Validation Error: No shader modules were added to VKLShaderCreateInfo!\n");
		return false;
	}
	
	return true;
}

VKLDescriptorSetLayoutCreateInfo::VKLDescriptorSetLayoutCreateInfo(VKLShaderCreateInfo& parent) : m_parent(parent){
	
}

VKLDescriptorSetLayoutCreateInfo& VKLDescriptorSetLayoutCreateInfo::addBinding(uint32_t binding, VkDescriptorType type,
																			   uint32_t count, VkShaderStageFlags stage) {
	VkDescriptorSetLayoutBinding result;
	result.binding = binding;
	result.descriptorType = type;
	result.descriptorCount = count;
	result.stageFlags = stage;
	result.pImmutableSamplers = NULL;
	
	m_bindings.push_back(result);
	
	return *this;
}

VKLShaderCreateInfo& VKLDescriptorSetLayoutCreateInfo::end() {
	return m_parent;
}
