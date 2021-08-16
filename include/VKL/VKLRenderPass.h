#ifndef VKLRenderPass_h
#define VKLRenderPass_h

#include <VKL/VKL_base.h>

class VKLRenderPassCreateInfo;

class VKLSubpassDescription {
public:
	VKLSubpassDescription& addInputAttachment(uint32_t attachment, VkImageLayout layout);
	VKLSubpassDescription& addColorAttachment(uint32_t attachment, VkImageLayout layout);
	VKLSubpassDescription& depthAttachment(uint32_t attachment, VkImageLayout layout);
	VKLSubpassDescription& preserveAttachment(uint32_t attachment);

	VKLRenderPassCreateInfo& end();

private:
	VKLSubpassDescription(VKLRenderPassCreateInfo& parent);

	VKLRenderPassCreateInfo& m_parent;

	VkSubpassDescription m_desc;

	std::vector<VkAttachmentReference> m_inputAttachments;
	std::vector<VkAttachmentReference> m_colorAttachments;
	std::vector<uint32_t> m_preserveAttachments;

	VkAttachmentReference m_depthAttachment;

	friend class VKLRenderPassCreateInfo;
};

class VKLAttachmentDescription {
public:
	VKLAttachmentDescription& samples(VkSampleCountFlagBits samples);
	VKLAttachmentDescription& op(VkAttachmentLoadOp load, VkAttachmentStoreOp store);
	VKLAttachmentDescription& stencilOp(VkAttachmentLoadOp load, VkAttachmentStoreOp store);
	VKLAttachmentDescription& layout(VkImageLayout initialLayout, VkImageLayout finalLayout);

	VKLRenderPassCreateInfo& end();

private:
	VKLAttachmentDescription(VKLRenderPassCreateInfo& parent, VkFormat format);

	VKLRenderPassCreateInfo& m_parent;

	VkAttachmentDescription m_desc;

	friend class VKLRenderPassCreateInfo;
};

class VKLRenderPassCreateInfo : public VKLCreateInfo<VKLRenderPassCreateInfo> {
public:
	VKLRenderPassCreateInfo();

	VKLRenderPassCreateInfo& device(const VKLDevice* device);
	VKLAttachmentDescription& addAttachment(VkFormat format);
	VKLSubpassDescription& addSubpass();

private:
	const VKLDevice* m_device;

	bool _validate();

	VkRenderPassCreateInfo m_createInfo;
	
	std::vector<VkAttachmentDescription> m_attachmentDescriptionsBuffer;
	std::vector<VkSubpassDescription> m_subpassDecriptionBuffer;

	std::vector<VKLAttachmentDescription> m_attachmentDescriptions;
	std::vector<VKLSubpassDescription> m_subpassDescriptions;

	friend class VKLSubpassDescription;
	friend class VKLAttachmentDescription;
	friend class VKLRenderPass;
};

class VKLRenderPass : public VKLHandle<VkRenderPass>, public VKLCreator<VKLRenderPassCreateInfo> {
public:
	VKLRenderPass();
	VKLRenderPass(const VKLRenderPassCreateInfo& createInfo);

private:
	const VKLDevice* m_device;

	void _destroy();
	void _create(const VKLRenderPassCreateInfo& createInfo);
};

#endif