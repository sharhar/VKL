#include <VKL/VKL.h>

int vklCreateShader(VKLDevice* device, VKLShader** pShader, VKLShaderCreateInfo* shaderCreateInfo) {
	VKLShader* shader = malloc_c(sizeof(VKLShader));

	shader->shaderModulesCount = shaderCreateInfo->shaderCount;
	shader->shaderModules = malloc_c(sizeof(VkShaderModule) * shaderCreateInfo->shaderCount);

	shader->shaderStageCreateInfos = malloc_c(sizeof(VkPipelineShaderStageCreateInfo) * shaderCreateInfo->shaderCount);

	for (int i = 0; i < shaderCreateInfo->shaderCount;i++) {
		size_t shaderCodeSize;
		char* shaderCode = readFileFromPath(shaderCreateInfo->shaderPaths[i], &shaderCodeSize);

		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		memset(&shaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = shaderCodeSize;
		shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode;

		VLKCheck(device->pvkCreateShaderModule(device->device, &shaderModuleCreateInfo, device->instance->allocator, &shader->shaderModules[i]),
			"Failed to create shader module");

		shader->shaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader->shaderStageCreateInfos[i].stage = shaderCreateInfo->shaderStages[i];
		shader->shaderStageCreateInfos[i].module = shader->shaderModules[i];
		shader->shaderStageCreateInfos[i].pName = "main";
		shader->shaderStageCreateInfos[i].pSpecializationInfo = NULL;
	}

	shader->descriptorPoolSizesCount = shaderCreateInfo->descriptorPoolSizesCount;
	shader->descriptorPoolSizes = shaderCreateInfo->decriptorPoolSizes;

	VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo;
	memset(&setLayoutCreateInfo, 0, sizeof(VkDescriptorSetLayoutCreateInfo));
	setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setLayoutCreateInfo.bindingCount = shaderCreateInfo->bindingsCount;
	setLayoutCreateInfo.pBindings = shaderCreateInfo->bindings;

	VLKCheck(device->pvkCreateDescriptorSetLayout(device->device, &setLayoutCreateInfo, device->instance->allocator, &shader->descriptorSetLayout),
		"Failed to create DescriptorSetLayout");

	VkVertexInputBindingDescription* vertexBindingDescription = malloc_c(sizeof(VkVertexInputBindingDescription));
	vertexBindingDescription->binding = 0;
	vertexBindingDescription->stride = shaderCreateInfo->vertexInputAttributeStride;
	vertexBindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription* vertexAttributeDescritpion = malloc_c(sizeof(VkVertexInputAttributeDescription) * shaderCreateInfo->vertexInputAttributesCount);
	for (int i = 0; i < shaderCreateInfo->vertexInputAttributesCount; i++) {
		vertexAttributeDescritpion[i].location = i;
		vertexAttributeDescritpion[i].binding = 0;
		vertexAttributeDescritpion[i].format = shaderCreateInfo->vertexInputAttributeFormats[i];
		vertexAttributeDescritpion[i].offset = shaderCreateInfo->vertexInputAttributeOffsets[i];
	}

	shader->vertexInputStateCreateInfo = malloc_c(sizeof(VkPipelineVertexInputStateCreateInfo));
	shader->vertexInputStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	shader->vertexInputStateCreateInfo->vertexBindingDescriptionCount = 1;
	shader->vertexInputStateCreateInfo->pVertexBindingDescriptions = vertexBindingDescription;
	shader->vertexInputStateCreateInfo->vertexAttributeDescriptionCount = shaderCreateInfo->vertexInputAttributesCount;
	shader->vertexInputStateCreateInfo->pVertexAttributeDescriptions = vertexAttributeDescritpion;

	shader->descriptorPoolSizesCount = shaderCreateInfo->bindingsCount;
	shader->descriptorPoolSizes = malloc_c(sizeof(VkDescriptorPoolSize) * shader->descriptorPoolSizesCount);

	for (uint32_t i = 0; i < shader->descriptorPoolSizesCount;i++) {
		shader->descriptorPoolSizes[i].type = shaderCreateInfo->bindings[i].descriptorType;
		shader->descriptorPoolSizes[i].descriptorCount = shaderCreateInfo->bindings[i].descriptorCount;
	}

	*pShader = shader;

	return 0;
}

int vklDestroyShader(VKLDevice* device, VKLShader* shader) {
	for (int i = 0; i < shader->shaderModulesCount;i++) {
		device->pvkDestroyShaderModule(device->device, shader->shaderModules[i], device->instance->allocator);
	}

	device->pvkDestroyDescriptorSetLayout(device->device, shader->descriptorSetLayout, device->instance->allocator);

	free_c(shader);

	return 0;
}
