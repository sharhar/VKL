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

class VKLInstance;
class VKLPhysicalDevice;
class VKLDevice;
class VKLQueue;
class VKLImage;
class VKLSwapChain;
class VKLCommandBuffer;
class VKLRenderTarget;
class VKLBuffer;
class VKLShader;
class VKLPipeline;

template<typename T>
class VKLHandle {
public:
	VKLHandle() { m_handle = VK_NULL_HANDLE;}
	T handle() const { return m_handle; }
protected:
	T m_handle;
};

class VKLCreateInfo {
public:
	virtual bool validate() = 0;
};

template<typename T>
class VKLBuilder {
public:
	VKLBuilder(const char* name) : m_name(name) { m_built = VK_FALSE; }
	VkBool32 built() const { return m_built; }
	void build(const T& createInfo) {
		if(((VKLCreateInfo*)(&createInfo))->validate()) {
			_build(createInfo);
			m_built = VK_TRUE;
		} else {
			printf("%s could not be built because the createInfo was not filled in.\n", m_name);
		}
	}
	
	virtual void destroy() = 0;
protected:
	VkBool32 m_built;
	const char* m_name;
	
	virtual void _build(const T& ci) = 0;
};

#endif
