//
//  VKLPipeline.h
//  VKL
//
//  Created by Shahar Sandhaus on 8/6/21.
//

#ifndef VKLPipeline_h
#define VKLPipeline_h

#include "VKL_base.h"

class VKLPipelineVertexInputStateCreateInfo;
class VKLPipelineCreateInfo;

class VKLVertexInputBindingDesc {
public:
	VKLVertexInputBindingDesc& inputRate(VkVertexInputRate inputRate);
	VKLVertexInputBindingDesc& addAttrib(uint32_t location, VkFormat format, uint32_t offset);

	VKLPipelineVertexInputStateCreateInfo& end();

private:
	VKLVertexInputBindingDesc(uint32_t binding, uint32_t stride, VKLPipelineVertexInputStateCreateInfo& parent);

	VkVertexInputBindingDescription m_desc;

	VKLPipelineVertexInputStateCreateInfo& m_parent;

	friend class VKLPipelineVertexInputStateCreateInfo;
};

class VKLPipelineVertexInputStateCreateInfo {
public:
	VKLVertexInputBindingDesc& addBinding(uint32_t binding, uint32_t stride);

	VKLPipelineCreateInfo& end();

private:
	VKLPipelineVertexInputStateCreateInfo(VKLPipelineCreateInfo& parent);

	void addVertexAttrib(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

	bool validate();

	std::vector<VKLVertexInputBindingDesc> m_vertexInputBindings;

	std::vector<VkVertexInputAttributeDescription> m_vertexAttributes;
	VkVertexInputBindingDescription* m_vertexInputBindingsBuffer;
	VkPipelineVertexInputStateCreateInfo m_createInfo;

	VKLPipelineCreateInfo& m_parent;

	friend class VKLVertexInputBindingDesc;
	friend class VKLPipelineCreateInfo;
	friend class VKLPipeline;
};

class VKLPipelineCreateInfo : public VKLCreateInfo<VKLPipelineCreateInfo> {
public:
	VKLPipelineCreateInfo();
	
	VKLPipelineCreateInfo& shader(const VKLShader* shader);
	VKLPipelineCreateInfo& renderTarget(const VKLRenderTarget* renderTarget);

	VKLPipelineVertexInputStateCreateInfo vertexInput;

private:
	const VKLShader* m_shader;
	const VKLRenderTarget* m_renderTarget;

	bool _validate();

	friend class VKLPipeline;
};

class VKLPipeline : public VKLHandle<VkPipeline>, public VKLCreator<VKLPipelineCreateInfo> {
public:
	VKLPipeline();
private:
	const VKLDevice* m_device;
	
	void _destroy();
	void _create(const VKLPipelineCreateInfo& createInfo);
};

#endif /* VKLPipeline_h */
