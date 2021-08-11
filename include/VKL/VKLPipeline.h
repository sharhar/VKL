//
//  VKLPipeline.h
//  VKL
//
//  Created by Shahar Sandhaus on 8/6/21.
//

#ifndef VKLPipeline_h
#define VKLPipeline_h

#include "VKL_base.h"

class VKLPipelineCreateInfo : public VKLCreateInfo<VKLPipelineCreateInfo> {
public:
	VKLPipelineCreateInfo();
	
	VKLPipelineCreateInfo& setShader(const VKLShader* shader);
	VKLPipelineCreateInfo& setRenderTarget(const VKLRenderTarget* renderTarget);
	
	const VKLShader* shader;
	const VKLRenderTarget* renderTarget;

private:
	bool _validate();
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
