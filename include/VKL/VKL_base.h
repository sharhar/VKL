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
class VKLPhysicalDevice;
class VKLDevice;
class VKLQueue;
class VKLImage;
class VKLSwapChain;
class VKLCommandBuffer;
class VKLRenderTarget;
class VKLBuffer;

template<typename T>
class VKLHandle {
public:
	VKLHandle() { m_handle = VK_NULL_HANDLE;}
	T handle() { return m_handle; }
protected:
	T m_handle;
};

template<typename T>
class VKLBuilder {
public:
	VKLBuilder(char* name) { m_built = VK_FALSE; m_name = name; }
	VkBool32 built() { return m_built; }
	void build() {
		if(buildable()) {
			_build();
			m_built = VK_TRUE;
			
		} else {
			printf("%s could not be built because the createInfo was not filled in.\n", m_name);
		}
		
	}
	virtual bool buildable() = 0;
	virtual void destroy() = 0;
	
	T ci;
protected:
	VkBool32 m_built;
	char* m_name;
	
	virtual void _build() = 0;
};

#endif
