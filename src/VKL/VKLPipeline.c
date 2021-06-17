#include <VKL/VKL.h>

int vklCreateGraphicsPipeline(VKLDevice* device, VKLPipeline** pPipeline, VKLGraphicsPipelineCreateInfo* createInfo) {
	VKLPipeline* pipeline = malloc_c(sizeof(VKLPipeline));

	VkPipelineLayoutCreateInfo layoutCreateInfo;
	memset(&layoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 1;
	layoutCreateInfo.pSetLayouts = &createInfo->shader->descriptorSetLayout;
	layoutCreateInfo.pushConstantRangeCount = createInfo->pushConstantRangeCount;
	layoutCreateInfo.pPushConstantRanges = createInfo->pushConstantRanges;

	VLKCheck(device->pvkCreatePipelineLayout(device->device, &layoutCreateInfo, device->instance->allocator, &pipeline->pipelineLayout),
		"Failed to create pipeline layout");

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	memset(&inputAssemblyStateCreateInfo, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = createInfo->topology;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport;
	memset(&viewport, 0, sizeof(VkViewport));
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = createInfo->extent.width;
	viewport.height = createInfo->extent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	VkRect2D scissors;
	memset(&scissors, 0, sizeof(VkRect2D));
	scissors.offset.x = 0;
	scissors.offset.y = 0;
	scissors.extent.width = createInfo->extent.width;
	scissors.extent.height = createInfo->extent.height;

	VkPipelineViewportStateCreateInfo viewportState;
	memset(&viewportState, 0, sizeof(VkPipelineViewportStateCreateInfo));
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissors;

	VkPipelineRasterizationStateCreateInfo rasterizationState;
	memset(&rasterizationState, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.rasterizerDiscardEnable = VK_FALSE;
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationState.cullMode = createInfo->cullMode;
	rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthBiasEnable = VK_FALSE;
	rasterizationState.depthBiasConstantFactor = 0;
	rasterizationState.depthBiasClamp = 0;
	rasterizationState.depthBiasSlopeFactor = 0;
	rasterizationState.lineWidth = 1;

	VkPipelineMultisampleStateCreateInfo multisampleState;
	memset(&multisampleState, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.sampleShadingEnable = VK_FALSE;
	multisampleState.minSampleShading = 0;
	multisampleState.pSampleMask = NULL;
	multisampleState.alphaToCoverageEnable = VK_FALSE;
	multisampleState.alphaToOneEnable = VK_FALSE;

	VkStencilOpState noOPStencilState;
	memset(&noOPStencilState, 0, sizeof(VkStencilOpState));
	noOPStencilState.failOp = VK_STENCIL_OP_KEEP;
	noOPStencilState.passOp = VK_STENCIL_OP_KEEP;
	noOPStencilState.depthFailOp = VK_STENCIL_OP_KEEP;
	noOPStencilState.compareOp = VK_COMPARE_OP_ALWAYS;
	noOPStencilState.compareMask = 0;
	noOPStencilState.writeMask = 0;
	noOPStencilState.reference = 0;

	VkPipelineDepthStencilStateCreateInfo depthState;
	memset(&depthState, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));
	depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthState.depthTestEnable = VK_TRUE;
	depthState.depthWriteEnable = VK_TRUE;
	depthState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthState.depthBoundsTestEnable = VK_FALSE;
	depthState.stencilTestEnable = VK_FALSE;
	depthState.front = noOPStencilState;
	depthState.back = noOPStencilState;
	depthState.minDepthBounds = 0;
	depthState.maxDepthBounds = 0;

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
	memset(&colorBlendAttachmentState, 0, sizeof(VkPipelineColorBlendAttachmentState));
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.colorWriteMask = 0xf;

	VkPipelineColorBlendStateCreateInfo colorBlendState;
	memset(&colorBlendState, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = VK_FALSE;
	colorBlendState.logicOp = VK_LOGIC_OP_CLEAR;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorBlendAttachmentState;
	colorBlendState.blendConstants[0] = 0.0;
	colorBlendState.blendConstants[1] = 0.0;
	colorBlendState.blendConstants[2] = 0.0;
	colorBlendState.blendConstants[3] = 0.0;
	
	VkDynamicState dynamicState[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	memset(&dynamicStateCreateInfo, 0, sizeof(VkPipelineDynamicStateCreateInfo));
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicState;
	
	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	memset(&pipelineCreateInfo, 0, sizeof(VkGraphicsPipelineCreateInfo));
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = createInfo->shader->shaderModulesCount;
	pipelineCreateInfo.pStages = createInfo->shader->shaderStageCreateInfos;
	pipelineCreateInfo.pVertexInputState = createInfo->shader->vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = NULL;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pMultisampleState = &multisampleState;
	pipelineCreateInfo.pDepthStencilState = &depthState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineCreateInfo.layout = pipeline->pipelineLayout;
	pipelineCreateInfo.renderPass = createInfo->renderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = NULL;
	pipelineCreateInfo.basePipelineIndex = 0;

	VLKCheck(device->pvkCreateGraphicsPipelines(device->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, device->instance->allocator, &pipeline->pipeline),
		"Failed to create graphics pipeline");

	*pPipeline = pipeline;

	return 0;
}

int vklCreateComputePipeline(VKLDevice* device, VKLPipeline** pPipeline, VKLShader* shader) {
	VKLPipeline* pipeline = malloc_c(sizeof(VKLPipeline));

	VkPipelineLayoutCreateInfo layoutCreateInfo;
	memset(&layoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 1;
	layoutCreateInfo.pSetLayouts = &shader->descriptorSetLayout;
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = NULL;

	VLKCheck(device->pvkCreatePipelineLayout(device->device, &layoutCreateInfo, device->instance->allocator, &pipeline->pipelineLayout),
		"Failed to create pipeline layout");

	VkComputePipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = NULL;
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.layout = pipeline->pipelineLayout;
	pipelineCreateInfo.stage = shader->shaderStageCreateInfos[0];

	VLKCheck(device->pvkCreateComputePipelines(device->device, NULL, 1, &pipelineCreateInfo, NULL, &pipeline->pipeline),
		"Failed to create compute pipeline");

	*pPipeline = pipeline;

	return 0;
}

int vklDestroyPipeline(VKLDevice* device, VKLPipeline* pipeline) {
	device->pvkDestroyPipeline(device->device, pipeline->pipeline, device->instance->allocator);
	device->pvkDestroyPipelineLayout(device->device, pipeline->pipelineLayout, device->instance->allocator);

	return 0;
}
