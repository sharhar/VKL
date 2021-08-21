#ifndef VKL_base_h
#define VKL_base_h

#ifdef _DEBUG
#define VK_CALL(result) {VkResult ___result = result; if(___result != VK_SUCCESS) { printf("(VkResult = %d) " #result " in " __FUNCTION__ " in " __FILE__ "\n", ___result); }}
#endif

#ifndef _DEBUG
#define VK_CALL(result) result;
#endif

#define VKL_VALIDATION

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
class VKLImageView;
class VKLSwapChain;
class VKLCommandBuffer;
class VKLRenderPass;
class VKLBuffer;
class VKLShader;
class VKLPipeline;
class VKLRenderPass;
class VKLDescriptorSet;
class VKLFramebuffer;

template<typename T>
class VKLHandle {
public:
	VKLHandle() { m_handle = VK_NULL_HANDLE;}
	T handle() const { return m_handle; }
protected:
	T m_handle;
};

template<typename T>
class VKLCreateInfo {
public:
	VKLCreateInfo() { 
		m_valid = VK_FALSE; 
	}

	T& invalidate() {
		m_valid = VK_FALSE; 
		return *((T*)this); 
	}

	bool validate() { 
		if (m_valid) return true;
		bool valid = _validate(); 
		if (valid) m_valid = VK_TRUE;
		return valid;
	}

protected:
	virtual bool _validate() = 0;
	VkBool32 m_valid;
};

template<typename T>
class VKLCreator {
public:
	VKLCreator(const char* name) : m_name(name) {
		m_valid = VK_FALSE; 
	}

	VkBool32 valid() const { 
		return m_valid;
	}
	
	void create(const T& createInfo) {
		if(((VKLCreateInfo<T>*)(&createInfo))->validate()) {
			if (m_valid) {
				destroy();
			}
			_create(createInfo);
			m_valid = VK_TRUE;
		} else {
			printf("%s could not be built because the createInfo was not valid.\n", m_name);
		}
	}
	
	void destroy() {
		if (m_valid) {
			_destroy();
		}
		m_valid = VK_FALSE;
	}
protected:
	VkBool32 m_valid;
	const char* m_name;
	
	virtual void _destroy() = 0;
	virtual void _create(const T& createInfo) = 0;
};

#endif
