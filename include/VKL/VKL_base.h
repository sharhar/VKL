#ifndef VKL_base_h
#define VKL_base_h

#ifdef _DEBUG
#define VK_CALL(result) {VkResult ___result = result; if(___result == VK_SUCCESS) { printf("(VkResult = %d) " #result " in " __FUNCTION__ " in " __FILE__ "\n", ___result); }}
#endif

#ifndef _DEBUG
#define VK_CALL(result) result;
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct VKLQueueCreateInfo VKLQueueCreateInfo;
typedef struct VKLDeviceCreateInfo VKLDeviceCreateInfo;
typedef struct VKLImageCreateInfo VKLImageCreateInfo;

class VKLInstance;
class VKLSurface;
class VKLPhysicalDevice;
class VKLDevice;
class VKLQueue;
class VKLImage;
class VKLSwapChain;
class VKLCommandBuffer;
class VKLRenderTarget;
class VKLBuffer;

template<typename T>
class VKLObject {
public:
	VKLObject() { m_built = VK_FALSE; }
	T handle() { return m_handle; }
	VkBool32 built() { return m_built; }
	void build() { _build(); m_built = VK_TRUE; }
	virtual void destroy() = 0;
protected:
	T m_handle;
	VkBool32 m_built;
	
	virtual void _build() = 0;
};

#endif
