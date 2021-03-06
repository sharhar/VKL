#ifndef VKLDevice_h
#define VKLDevice_h

#include "VKL_base.h"

#include <vector>

typedef struct VKLDevicePFNS {
	PFN_vkCreateDevice CreateDevice;
	PFN_vkDestroyDevice DestroyDevice;
	PFN_vkGetDeviceProcAddr GetDeviceProcAddr;
	PFN_vkGetDeviceQueue GetDeviceQueue;
	PFN_vkQueueSubmit QueueSubmit;
	PFN_vkQueueWaitIdle QueueWaitIdle;
	PFN_vkDeviceWaitIdle DeviceWaitIdle;
	PFN_vkAllocateMemory AllocateMemory;
	PFN_vkFreeMemory FreeMemory;
	PFN_vkMapMemory MapMemory;
	PFN_vkUnmapMemory UnmapMemory;
	PFN_vkFlushMappedMemoryRanges FlushMappedMemoryRanges;
	PFN_vkInvalidateMappedMemoryRanges InvalidateMappedMemoryRanges;
	PFN_vkGetDeviceMemoryCommitment GetDeviceMemoryCommitment;
	PFN_vkBindBufferMemory BindBufferMemory;
	PFN_vkBindImageMemory BindImageMemory;
	PFN_vkGetBufferMemoryRequirements GetBufferMemoryRequirements;
	PFN_vkGetImageMemoryRequirements GetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements GetImageSparseMemoryRequirements;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties GetPhysicalDeviceSparseImageFormatProperties;
	PFN_vkQueueBindSparse QueueBindSparse;
	PFN_vkCreateFence CreateFence;
	PFN_vkDestroyFence DestroyFence;
	PFN_vkResetFences ResetFences;
	PFN_vkGetFenceStatus GetFenceStatus;
	PFN_vkWaitForFences WaitForFences;
	PFN_vkCreateSemaphore CreateSemaphore;
	PFN_vkDestroySemaphore DestroySemaphore;
	PFN_vkCreateEvent CreateEvent;
	PFN_vkDestroyEvent DestroyEvent;
	PFN_vkGetEventStatus GetEventStatus;
	PFN_vkSetEvent SetEvent;
	PFN_vkResetEvent ResetEvent;
	PFN_vkCreateQueryPool CreateQueryPool;
	PFN_vkDestroyQueryPool DestroyQueryPool;
	PFN_vkGetQueryPoolResults GetQueryPoolResults;
	PFN_vkCreateBuffer CreateBuffer;
	PFN_vkDestroyBuffer DestroyBuffer;
	PFN_vkCreateBufferView CreateBufferView;
	PFN_vkDestroyBufferView DestroyBufferView;
	PFN_vkCreateImage CreateImage;
	PFN_vkDestroyImage DestroyImage;
	PFN_vkGetImageSubresourceLayout GetImageSubresourceLayout;
	PFN_vkCreateImageView CreateImageView;
	PFN_vkDestroyImageView DestroyImageView;
	PFN_vkCreateShaderModule CreateShaderModule;
	PFN_vkDestroyShaderModule DestroyShaderModule;
	PFN_vkCreatePipelineCache CreatePipelineCache;
	PFN_vkDestroyPipelineCache DestroyPipelineCache;
	PFN_vkGetPipelineCacheData GetPipelineCacheData;
	PFN_vkMergePipelineCaches MergePipelineCaches;
	PFN_vkCreateGraphicsPipelines CreateGraphicsPipelines;
	PFN_vkCreateComputePipelines CreateComputePipelines;
	PFN_vkDestroyPipeline DestroyPipeline;
	PFN_vkCreatePipelineLayout CreatePipelineLayout;
	PFN_vkDestroyPipelineLayout DestroyPipelineLayout;
	PFN_vkCreateSampler CreateSampler;
	PFN_vkDestroySampler DestroySampler;
	PFN_vkCreateDescriptorSetLayout CreateDescriptorSetLayout;
	PFN_vkDestroyDescriptorSetLayout DestroyDescriptorSetLayout;
	PFN_vkCreateDescriptorPool CreateDescriptorPool;
	PFN_vkDestroyDescriptorPool DestroyDescriptorPool;
	PFN_vkResetDescriptorPool ResetDescriptorPool;
	PFN_vkAllocateDescriptorSets AllocateDescriptorSets;
	PFN_vkFreeDescriptorSets FreeDescriptorSets;
	PFN_vkUpdateDescriptorSets UpdateDescriptorSets;
	PFN_vkCreateFramebuffer CreateFramebuffer;
	PFN_vkDestroyFramebuffer DestroyFramebuffer;
	PFN_vkCreateRenderPass CreateRenderPass;
	PFN_vkDestroyRenderPass DestroyRenderPass;
	PFN_vkGetRenderAreaGranularity GetRenderAreaGranularity;
	PFN_vkCreateCommandPool CreateCommandPool;
	PFN_vkDestroyCommandPool DestroyCommandPool;
	PFN_vkResetCommandPool ResetCommandPool;
	PFN_vkAllocateCommandBuffers AllocateCommandBuffers;
	PFN_vkFreeCommandBuffers FreeCommandBuffers;
	PFN_vkBeginCommandBuffer BeginCommandBuffer;
	PFN_vkEndCommandBuffer EndCommandBuffer;
	PFN_vkResetCommandBuffer ResetCommandBuffer;
	PFN_vkCmdBindPipeline CmdBindPipeline;
	PFN_vkCmdSetViewport CmdSetViewport;
	PFN_vkCmdSetScissor CmdSetScissor;
	PFN_vkCmdSetLineWidth CmdSetLineWidth;
	PFN_vkCmdSetDepthBias CmdSetDepthBias;
	PFN_vkCmdSetBlendConstants CmdSetBlendConstants;
	PFN_vkCmdSetDepthBounds CmdSetDepthBounds;
	PFN_vkCmdSetStencilCompareMask CmdSetStencilCompareMask;
	PFN_vkCmdSetStencilWriteMask CmdSetStencilWriteMask;
	PFN_vkCmdSetStencilReference CmdSetStencilReference;
	PFN_vkCmdBindDescriptorSets CmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer CmdBindIndexBuffer;
	PFN_vkCmdBindVertexBuffers CmdBindVertexBuffers;
	PFN_vkCmdDraw CmdDraw;
	PFN_vkCmdDrawIndexed CmdDrawIndexed;
	PFN_vkCmdDrawIndirect CmdDrawIndirect;
	PFN_vkCmdDrawIndexedIndirect CmdDrawIndexedIndirect;
	PFN_vkCmdDispatch CmdDispatch;
	PFN_vkCmdDispatchIndirect CmdDispatchIndirect;
	PFN_vkCmdCopyBuffer CmdCopyBuffer;
	PFN_vkCmdCopyImage CmdCopyImage;
	PFN_vkCmdBlitImage CmdBlitImage;
	PFN_vkCmdCopyBufferToImage CmdCopyBufferToImage;
	PFN_vkCmdCopyImageToBuffer CmdCopyImageToBuffer;
	PFN_vkCmdUpdateBuffer CmdUpdateBuffer;
	PFN_vkCmdFillBuffer CmdFillBuffer;
	PFN_vkCmdClearColorImage CmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage CmdClearDepthStencilImage;
	PFN_vkCmdClearAttachments CmdClearAttachments;
	PFN_vkCmdResolveImage CmdResolveImage;
	PFN_vkCmdSetEvent CmdSetEvent;
	PFN_vkCmdResetEvent CmdResetEvent;
	PFN_vkCmdWaitEvents CmdWaitEvents;
	PFN_vkCmdPipelineBarrier CmdPipelineBarrier;
	PFN_vkCmdBeginQuery CmdBeginQuery;
	PFN_vkCmdEndQuery CmdEndQuery;
	PFN_vkCmdResetQueryPool CmdResetQueryPool;
	PFN_vkCmdWriteTimestamp CmdWriteTimestamp;
	PFN_vkCmdCopyQueryPoolResults CmdCopyQueryPoolResults;
	PFN_vkCmdPushConstants CmdPushConstants;
	PFN_vkCmdBeginRenderPass CmdBeginRenderPass;
	PFN_vkCmdNextSubpass CmdNextSubpass;
	PFN_vkCmdEndRenderPass CmdEndRenderPass;
	PFN_vkCmdExecuteCommands CmdExecuteCommands;

	PFN_vkCreateSwapchainKHR CreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR DestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR GetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR AcquireNextImageKHR;
	PFN_vkQueuePresentKHR QueuePresentKHR;
} VKLDevicePFNS;

typedef struct VKLDeviceCreateInfo {
	VKLDeviceCreateInfo(VKLInstance* instance, VKLPhysicalDevice* physicalDevice, VKLQueueCreateInfo* queueCreateInfo);

	void addExtension(char* extension);

	VKLInstance* instance;
	VKLPhysicalDevice* physicalDevice;
	VKLQueueCreateInfo* queueCreateInfo;
	std::vector<char*> extensions;
} VKLDeviceCreateInfo;

class VKLDevice {
public:
	VKLDevice(VKLDeviceCreateInfo* createInfo);

	VkDevice handle();
	VkAllocationCallbacks* allocator();
	PFN_vkVoidFunction procAddr(const char* name);
	VKLQueue& getQueue(uint32_t typeIndex, uint32_t queueIndex);
	void destroy();

	VKLDevicePFNS vk;
private:
	VkDevice m_handle;

	VKLQueue** m_queues;
	uint32_t m_queuesCount;

	VKLInstance* m_instance;
	VKLPhysicalDevice* m_physicalDevice;
	VkAllocationCallbacks* m_allocator;
};

#endif