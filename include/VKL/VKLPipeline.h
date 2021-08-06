//
//  VKLPipeline.h
//  VKL
//
//  Created by Shahar Sandhaus on 8/6/21.
//

#ifndef VKLPipeline_h
#define VKLPipeline_h

#include <VKL/VKL_Base.h>

class VKLPipelineCreateInfo : public VKLCreateInfo {
public:
	VKLPipelineCreateInfo();
	
	VKLPipelineCreateInfo& setShader(const VKLShader* shader);
	VKLPipelineCreateInfo& setRenderTarget(const VKLRenderTarget* renderTarget);
	
	bool validate();
	
	const VKLShader* shader;
	const VKLRenderTarget* renderTarget;
};

class VKLPipeline : public VKLHandle<VkPipeline>, public VKLBuilder<VKLPipelineCreateInfo> {
public:
	VKLPipeline();
	
	void destroy();
private:
	const VKLDevice* m_device;
	
	void _build(const VKLPipelineCreateInfo& createInfo);
};

#endif /* VKLPipeline_h */
