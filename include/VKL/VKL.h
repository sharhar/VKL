#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void VLKCheck(VkResult result, char *msg);
void* malloc_c(size_t size);
void free_c(void* block);
void* realloc_c(void* pOriginal, size_t size);
char* readFileFromPath(char *filename, size_t* size);

typedef void*(*PFN_vklLoadFunct)(VkInstance* instance, char* name);

struct VKLInstance;
struct VKLSurface;
struct VKLDevice;
struct VKLDeviceGraphicsContext;
struct VKLDeviceComputeContext;
struct VKLSwapChain;
struct VKLBuffer;
struct VKLShader;
struct VKLPipeline;
struct VKLUniformObject;
struct VKLTexture;
struct VKLFrameBuffer;

typedef struct VKLInstance VKLInstance;
typedef struct VKLSurface VKLSurface;
typedef struct VKLDevice VKLDevice;
typedef struct VKLDeviceGraphicsContext VKLDeviceGraphicsContext;
typedef struct VKLDeviceComputeContext VKLDeviceComputeContext;
typedef struct VKLSwapChain VKLSwapChain;
typedef struct VKLBuffer VKLBuffer;
typedef struct VKLShader VKLShader;
typedef struct VKLPipeline VKLPipeline;
typedef struct VKLUniformObject VKLUniformObject;
typedef struct VKLTexture VKLTexture;
typedef struct VKLFrameBuffer VKLFrameBuffer;

typedef struct VKLInstance {
	PFN_vkCreateInstance pvkCreateInstance;
	PFN_vkDestroyInstance pvkDestroyInstance;
	PFN_vkEnumeratePhysicalDevices pvkEnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceFeatures pvkGetPhysicalDeviceFeatures;
	PFN_vkGetPhysicalDeviceFormatProperties pvkGetPhysicalDeviceFormatProperties;
	PFN_vkGetPhysicalDeviceImageFormatProperties pvkGetPhysicalDeviceImageFormatProperties;
	PFN_vkGetPhysicalDeviceProperties pvkGetPhysicalDeviceProperties;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties pvkGetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceMemoryProperties pvkGetPhysicalDeviceMemoryProperties;
	PFN_vkGetInstanceProcAddr pvkGetInstanceProcAddr;
	PFN_vkCreateDevice pvkCreateDevice;
	PFN_vkDestroyDevice pvkDestroyDevice;
	PFN_vkEnumerateInstanceExtensionProperties pvkEnumerateInstanceExtensionProperties;
	PFN_vkEnumerateDeviceExtensionProperties pvkEnumerateDeviceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties pvkEnumerateInstanceLayerProperties;
	PFN_vkEnumerateDeviceLayerProperties pvkEnumerateDeviceLayerProperties;

	PFN_vkCreateDebugReportCallbackEXT pvkCreateDebugReportCallbackEXT;
	PFN_vkDestroyDebugReportCallbackEXT pvkDestroyDebugReportCallbackEXT;

	PFN_vkDestroySurfaceKHR pvkDestroySurfaceKHR;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR pvkGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pvkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pvkGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pvkGetPhysicalDeviceSurfacePresentModesKHR;

	PFN_vkGetPhysicalDeviceDisplayPropertiesKHR pvkGetPhysicalDeviceDisplayPropertiesKHR;
	PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR pvkGetPhysicalDeviceDisplayPlanePropertiesKHR;
	PFN_vkGetDisplayPlaneSupportedDisplaysKHR pvkGetDisplayPlaneSupportedDisplaysKHR;
	PFN_vkGetDisplayModePropertiesKHR pvkGetDisplayModePropertiesKHR;
	PFN_vkCreateDisplayModeKHR pvkCreateDisplayModeKHR;
	PFN_vkGetDisplayPlaneCapabilitiesKHR pvkGetDisplayPlaneCapabilitiesKHR;
	PFN_vkCreateDisplayPlaneSurfaceKHR pvkCreateDisplayPlaneSurfaceKHR;

	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkAllocationCallbacks* allocator;

	int layerCount;
	char** layers;

	VkBool32 debug;
} VKLInstance;

typedef struct VKLSurface {
	VkSurfaceKHR surface;
	uint32_t width, height;
} VKLSurface;

typedef struct VKLDevice {
	PFN_vkGetDeviceProcAddr pvkGetDeviceProcAddr;
	PFN_vkGetDeviceQueue pvkGetDeviceQueue;
	PFN_vkQueueSubmit pvkQueueSubmit;
	PFN_vkQueueWaitIdle pvkQueueWaitIdle;
	PFN_vkDeviceWaitIdle pvkDeviceWaitIdle;
	PFN_vkAllocateMemory pvkAllocateMemory;
	PFN_vkFreeMemory pvkFreeMemory;
	PFN_vkMapMemory pvkMapMemory;
	PFN_vkUnmapMemory pvkUnmapMemory;
	PFN_vkFlushMappedMemoryRanges pvkFlushMappedMemoryRanges;
	PFN_vkInvalidateMappedMemoryRanges pvkInvalidateMappedMemoryRanges;
	PFN_vkGetDeviceMemoryCommitment pvkGetDeviceMemoryCommitment;
	PFN_vkBindBufferMemory pvkBindBufferMemory;
	PFN_vkBindImageMemory pvkBindImageMemory;
	PFN_vkGetBufferMemoryRequirements pvkGetBufferMemoryRequirements;
	PFN_vkGetImageMemoryRequirements pvkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements pvkGetImageSparseMemoryRequirements;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties pvkGetPhysicalDeviceSparseImageFormatProperties;
	PFN_vkQueueBindSparse pvkQueueBindSparse;
	PFN_vkCreateFence pvkCreateFence;
	PFN_vkDestroyFence pvkDestroyFence;
	PFN_vkResetFences pvkResetFences;
	PFN_vkGetFenceStatus pvkGetFenceStatus;
	PFN_vkWaitForFences pvkWaitForFences;
	PFN_vkCreateSemaphore pvkCreateSemaphore;
	PFN_vkDestroySemaphore pvkDestroySemaphore;
	PFN_vkCreateEvent pvkCreateEvent;
	PFN_vkDestroyEvent pvkDestroyEvent;
	PFN_vkGetEventStatus pvkGetEventStatus;
	PFN_vkSetEvent pvkSetEvent;
	PFN_vkResetEvent pvkResetEvent;
	PFN_vkCreateQueryPool pvkCreateQueryPool;
	PFN_vkDestroyQueryPool pvkDestroyQueryPool;
	PFN_vkGetQueryPoolResults pvkGetQueryPoolResults;
	PFN_vkCreateBuffer pvkCreateBuffer;
	PFN_vkDestroyBuffer pvkDestroyBuffer;
	PFN_vkCreateBufferView pvkCreateBufferView;
	PFN_vkDestroyBufferView pvkDestroyBufferView;
	PFN_vkCreateImage pvkCreateImage;
	PFN_vkDestroyImage pvkDestroyImage;
	PFN_vkGetImageSubresourceLayout pvkGetImageSubresourceLayout;
	PFN_vkCreateImageView pvkCreateImageView;
	PFN_vkDestroyImageView pvkDestroyImageView;
	PFN_vkCreateShaderModule pvkCreateShaderModule;
	PFN_vkDestroyShaderModule pvkDestroyShaderModule;
	PFN_vkCreatePipelineCache pvkCreatePipelineCache;
	PFN_vkDestroyPipelineCache pvkDestroyPipelineCache;
	PFN_vkGetPipelineCacheData pvkGetPipelineCacheData;
	PFN_vkMergePipelineCaches pvkMergePipelineCaches;
	PFN_vkCreateGraphicsPipelines pvkCreateGraphicsPipelines;
	PFN_vkCreateComputePipelines pvkCreateComputePipelines;
	PFN_vkDestroyPipeline pvkDestroyPipeline;
	PFN_vkCreatePipelineLayout pvkCreatePipelineLayout;
	PFN_vkDestroyPipelineLayout pvkDestroyPipelineLayout;
	PFN_vkCreateSampler pvkCreateSampler;
	PFN_vkDestroySampler pvkDestroySampler;
	PFN_vkCreateDescriptorSetLayout pvkCreateDescriptorSetLayout;
	PFN_vkDestroyDescriptorSetLayout pvkDestroyDescriptorSetLayout;
	PFN_vkCreateDescriptorPool pvkCreateDescriptorPool;
	PFN_vkDestroyDescriptorPool pvkDestroyDescriptorPool;
	PFN_vkResetDescriptorPool pvkResetDescriptorPool;
	PFN_vkAllocateDescriptorSets pvkAllocateDescriptorSets;
	PFN_vkFreeDescriptorSets pvkFreeDescriptorSets;
	PFN_vkUpdateDescriptorSets pvkUpdateDescriptorSets;
	PFN_vkCreateFramebuffer pvkCreateFramebuffer;
	PFN_vkDestroyFramebuffer pvkDestroyFramebuffer;
	PFN_vkCreateRenderPass pvkCreateRenderPass;
	PFN_vkDestroyRenderPass pvkDestroyRenderPass;
	PFN_vkGetRenderAreaGranularity pvkGetRenderAreaGranularity;
	PFN_vkCreateCommandPool pvkCreateCommandPool;
	PFN_vkDestroyCommandPool pvkDestroyCommandPool;
	PFN_vkResetCommandPool pvkResetCommandPool;
	PFN_vkAllocateCommandBuffers pvkAllocateCommandBuffers;
	PFN_vkFreeCommandBuffers pvkFreeCommandBuffers;
	PFN_vkBeginCommandBuffer pvkBeginCommandBuffer;
	PFN_vkEndCommandBuffer pvkEndCommandBuffer;
	PFN_vkResetCommandBuffer pvkResetCommandBuffer;
	PFN_vkCmdBindPipeline pvkCmdBindPipeline;
	PFN_vkCmdSetViewport pvkCmdSetViewport;
	PFN_vkCmdSetScissor pvkCmdSetScissor;
	PFN_vkCmdSetLineWidth pvkCmdSetLineWidth;
	PFN_vkCmdSetDepthBias pvkCmdSetDepthBias;
	PFN_vkCmdSetBlendConstants pvkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBounds pvkCmdSetDepthBounds;
	PFN_vkCmdSetStencilCompareMask pvkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilWriteMask pvkCmdSetStencilWriteMask;
	PFN_vkCmdSetStencilReference pvkCmdSetStencilReference;
	PFN_vkCmdBindDescriptorSets pvkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer pvkCmdBindIndexBuffer;
	PFN_vkCmdBindVertexBuffers pvkCmdBindVertexBuffers;
	PFN_vkCmdDraw pvkCmdDraw;
	PFN_vkCmdDrawIndexed pvkCmdDrawIndexed;
	PFN_vkCmdDrawIndirect pvkCmdDrawIndirect;
	PFN_vkCmdDrawIndexedIndirect pvkCmdDrawIndexedIndirect;
	PFN_vkCmdDispatch pvkCmdDispatch;
	PFN_vkCmdDispatchIndirect pvkCmdDispatchIndirect;
	PFN_vkCmdCopyBuffer pvkCmdCopyBuffer;
	PFN_vkCmdCopyImage pvkCmdCopyImage;
	PFN_vkCmdBlitImage pvkCmdBlitImage;
	PFN_vkCmdCopyBufferToImage pvkCmdCopyBufferToImage;
	PFN_vkCmdCopyImageToBuffer pvkCmdCopyImageToBuffer;
	PFN_vkCmdUpdateBuffer pvkCmdUpdateBuffer;
	PFN_vkCmdFillBuffer pvkCmdFillBuffer;
	PFN_vkCmdClearColorImage pvkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage pvkCmdClearDepthStencilImage;
	PFN_vkCmdClearAttachments pvkCmdClearAttachments;
	PFN_vkCmdResolveImage pvkCmdResolveImage;
	PFN_vkCmdSetEvent pvkCmdSetEvent;
	PFN_vkCmdResetEvent pvkCmdResetEvent;
	PFN_vkCmdWaitEvents pvkCmdWaitEvents;
	PFN_vkCmdPipelineBarrier pvkCmdPipelineBarrier;
	PFN_vkCmdBeginQuery pvkCmdBeginQuery;
	PFN_vkCmdEndQuery pvkCmdEndQuery;
	PFN_vkCmdResetQueryPool pvkCmdResetQueryPool;
	PFN_vkCmdWriteTimestamp pvkCmdWriteTimestamp;
	PFN_vkCmdCopyQueryPoolResults pvkCmdCopyQueryPoolResults;
	PFN_vkCmdPushConstants pvkCmdPushConstants;
	PFN_vkCmdBeginRenderPass pvkCmdBeginRenderPass;
	PFN_vkCmdNextSubpass pvkCmdNextSubpass;
	PFN_vkCmdEndRenderPass pvkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands pvkCmdExecuteCommands;

	PFN_vkCreateSwapchainKHR pvkCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR pvkDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR pvkGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR pvkAcquireNextImageKHR;
	PFN_vkQueuePresentKHR pvkQueuePresentKHR;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	VkDevice device;

	VKLInstance* instance;
	uint32_t deviceGraphicsContextCount;
	uint32_t deviceComputeContextCount;
	VKLDeviceGraphicsContext** deviceGraphicsContexts;
	VKLDeviceComputeContext** deviceComputeContexts;
} VKLDevice;

typedef struct VKLDeviceGraphicsContext {
	uint32_t queueIdx;
	VkQueue queue;
	VkCommandPool commandPool;
	VkCommandBuffer setupCmdBuffer;

	VKLDevice* device;
	
	VKLSurface* surface;
} VKLDeviceGraphicsContext;

typedef struct VKLDeviceComputeContext {
	uint32_t queueIdx;
	VkQueue queue;
	VkCommandPool commandPool;
	VkCommandBuffer setupCmdBuffer;

	VKLDevice* device;
} VKLDeviceComputeContext;

typedef struct VKLSwapChain {
	VkSwapchainKHR swapChain;
	VkImage* presentImages;
	VkImageView* presentImageViews;
	VkSemaphore presentCompleteSemaphore, renderingCompleteSemaphore;

	VkCommandBuffer* cmdBuffers;
	VKLFrameBuffer* backBuffer;
	
	uint32_t width, height, nextImageIdx, imageCount;
	VkBool32 vSync;

	VKLDeviceGraphicsContext* context;
} VKLSwapChain;

typedef struct VKLBuffer {
	size_t size;
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkBool32 deviceLocal;
} VKLBuffer;

typedef struct VKLShader {
	uint32_t shaderModulesCount;
	VkShaderModule* shaderModules;
	VkDescriptorPoolSize* descriptorPoolSizes;
	uint32_t descriptorPoolSizesCount;
	VkDescriptorSetLayout descriptorSetLayout;

	uint32_t shaderStageCreateInfosCount;
	VkPipelineShaderStageCreateInfo* shaderStageCreateInfos;
	VkPipelineVertexInputStateCreateInfo* vertexInputStateCreateInfo;
} VKLShader;

typedef struct VKLPipeline {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
} VKLPipeline;

typedef struct VKLUniformObject {
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
} VKLUniformObject;

typedef struct VKLTexture {
	uint32_t width;
	uint32_t height;
	size_t colorSize;
	uint32_t colorCount;

	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	VkSampler sampler;

	VkAccessFlags accessMask;
	VkImageLayout layout;

	VkBool32 temporary;
} VKLTexture;

typedef struct VKLFrameBuffer {
	uint32_t width;
	uint32_t height;

	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	VkRenderPass renderPass;
	VkFramebuffer frameBuffer;
	
	VkSampler sampler;

	float clearR, clearG, clearB, clearA;

	VkFormat imageFormat;
	VkAccessFlags accessMask;
	VkImageLayout layout;
} VKLFrameBuffer;

int vklCreateInstance(VKLInstance** pInstace, VkAllocationCallbacks* allocator, char* wsiExtName, PFN_vkGetInstanceProcAddr vkFunct, VkBool32 debug);
int vklDestroyInstance(VKLInstance* instance);

int vklDestroySurface(VKLInstance* instance, VKLSurface* surface);

int vklCreateDevice(VKLInstance* instance, VKLDevice** pDevice, VKLSurface** pSurfaces,
	uint32_t deviceGraphicsContextCount, VKLDeviceGraphicsContext*** pDeviceGraphicsContexts,
	uint32_t deviceComputeContextCount, VKLDeviceComputeContext*** pDeviceComputeContexts);
int vklDestroyDevice(VKLDevice* device);

int vklAllocateCommandBuffer(VKLDeviceGraphicsContext* context, VkCommandBuffer* cmdBuffer, VkCommandBufferLevel level, uint32_t count);
int vklBeginCommandBuffer(VKLDevice* device, VkCommandBuffer cmdBuffer);
int vklEndCommandBuffer(VKLDevice* device, VkCommandBuffer cmdBuffer);
int vklExecuteCommandBuffer(VKLDeviceGraphicsContext* devCon, VkCommandBuffer cmdBuffer);

int vklCreateSwapChain(VKLDeviceGraphicsContext* context, VKLSwapChain** swapChain, VkBool32 vSync);
int vklRecreateSwapChain(VKLDeviceGraphicsContext* context, VKLSwapChain* swapChain);
int vklGetBackBuffer(VKLSwapChain* swapChain, VKLFrameBuffer** pFrameBuffer);
int vklDestroySwapChain(VKLSwapChain* swapChain);
int vklPresent(VKLSwapChain* swapChain);

int vklAllocateMemory(VKLDevice* device, VkDeviceMemory* memory, VkMemoryPropertyFlags desiredMemoryFlags, VkMemoryRequirements memoryRequirements);
int vklAllocateImageMemory(VKLDevice* device, VkDeviceMemory* memory, VkImage image, VkMemoryPropertyFlags desiredMemoryFlags);
int vklAllocateBufferMemory(VKLDevice* device, VkDeviceMemory* memory, VkBuffer buffer, VkMemoryPropertyFlags desiredMemoryFlags);
int vklWriteToMemory(VKLDevice* device, VkDeviceMemory memory, void* data, size_t size, size_t offset);
int vklReadFromMemory(VKLDevice* device, VkDeviceMemory memory, void* data, size_t size, size_t offset);

int vklCreateBuffer(VKLDevice* device, VKLBuffer** pBuffer, VkBool32 deviceLocal, size_t size, VkBufferUsageFlags usage);
int vklDestroyBuffer(VKLDevice* device, VKLBuffer* buffer);
int vklCreateStagedBuffer(VKLDeviceGraphicsContext* device, VKLBuffer** pBuffer, void* data, size_t size, VkBufferUsageFlags usage);

typedef struct VKLShaderCreateInfo {
	char** shaderPaths;
	VkShaderStageFlagBits* shaderStages;
	uint32_t shaderCount;
	VkDescriptorPoolSize* decriptorPoolSizes;
	uint32_t descriptorPoolSizesCount;
	VkDescriptorSetLayoutBinding* bindings;
	uint32_t bindingsCount;
	size_t* vertexInputAttributeOffsets;
	VkFormat* vertexInputAttributeFormats;
	uint32_t vertexInputAttributesCount;
	uint32_t* vertexInputAttributeBindings;
	size_t* vertexBindingStrides;
	VkVertexInputRate* vertexBindingInputRates;
	uint32_t vertexBindingsCount;
} VKLShaderCreateInfo;

int vklCreateShader(VKLDevice* device, VKLShader** pShader, VKLShaderCreateInfo* shaderCreateInfo);
int vklDestroyShader(VKLDevice* device, VKLShader* shader);

typedef struct VKLGraphicsPipelineCreateInfo {
	VKLShader* shader;
	VkPrimitiveTopology topology;
	VkCullModeFlags cullMode;
	VkRenderPass renderPass;
	VkExtent2D extent;
	VkPushConstantRange* pushConstantRanges;
	uint32_t pushConstantRangeCount;
} VKLGraphicsPipelineCreateInfo;

int vklCreateGraphicsPipeline(VKLDevice* device, VKLPipeline** pPipeline, VKLGraphicsPipelineCreateInfo* createInfo);
int vklCreateComputePipeline(VKLDevice* device, VKLPipeline** pPipeline, VKLShader* shader);
int vklDestroyPipeline(VKLDevice* device, VKLPipeline* pipeline);

int vklCreateUniformObject(VKLDevice* device, VKLUniformObject** pUniform, VKLShader* shader);
int vklSetUniformBuffer(VKLDevice* device, VKLUniformObject* uniform, VKLBuffer* buffer, uint32_t index);
int vklSetUniformStorageBuffer(VKLDevice* device, VKLUniformObject* uniform, VKLBuffer* buffer, uint32_t index);
int vklSetUniformTexture(VKLDevice* device, VKLUniformObject* uniform, VKLTexture* texture, uint32_t index);
int vklSetUniformFramebuffer(VKLDevice* device, VKLUniformObject* uniform, VKLFrameBuffer* framebuffer, uint32_t index);
int vklDestroyUniformObject(VKLDevice* device, VKLUniformObject* uniform);

typedef struct VKLTextureCreateInfo {
	uint32_t width;
	uint32_t height;
	VkImageUsageFlags usage;
	VkFormat format;
	VkImageTiling tiling;
	VkFilter filter;
	size_t colorSize;
	uint32_t colorCount;
} VKLTextureCreateInfo;

int vklCreateTexture(VKLDevice* device, VKLTexture** pTexture, VKLTextureCreateInfo* createInfo, VkBool32 deviceLocal);
int vklSetTextureData(VKLDevice* device, VKLTexture* texture, uint8_t* data);
int vklCreateStagedTexture(VKLDeviceGraphicsContext* devCon, VKLTexture** pTexture, VKLTextureCreateInfo* createInfo, uint8_t* data);
int vklImageLayoutTransition(VKLDevice* device, VKLTexture* texture, VkCommandBuffer cmdBuffer, VkAccessFlags accessMask, VkImageLayout layout);
int vklDestroyTexture(VKLDevice* device, VKLTexture* texture);

int vklCreateFrameBuffer(VKLDeviceGraphicsContext* devCon, VKLFrameBuffer** pFrameBuffer, uint32_t width, uint32_t height, VkFormat imageFormat, VkAccessFlags accessMask, VkImageLayout layout);
int vklRecreateFrameBuffer(VKLDeviceGraphicsContext* devCon, VKLFrameBuffer* framebuffer, int width, int height);
int vklSetClearColor(VKLFrameBuffer* frameBuffer, float r, float g, float b, float a);
int vklBeginRender(VKLDevice* device, VKLFrameBuffer* frameBuffer, VkCommandBuffer cmdBuffer);
int vklEndRender(VKLDevice* device, VKLFrameBuffer* frameBuffer, VkCommandBuffer cmdBuffer);
int vklDestroyFrameBuffer(VKLDevice* device, VKLFrameBuffer* frameBuffer);

#ifdef __cplusplus
}
#endif
