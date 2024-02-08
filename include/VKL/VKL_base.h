#ifndef VKL_base_h
#define VKL_base_h

//#ifdef _DEBUG
#define VK_CALL(result) {VkResult ___result = result; if(___result != VK_SUCCESS) { printf("(VkResult = %d) " #result " in %s in %s\n", ___result, __FUNCTION__, __FILE__); }}
//#endif

//#ifndef _DEBUG
//#define VK_CALL(result) result;
//#endif

#define VKL_VALIDATION

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

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
class VKLPipelineLayout;
class VKLPipeline;
class VKLComputePipeline;
class VKLRenderPass;
class VKLDescriptorSet;
class VKLFramebuffer;



inline void log_message(const char* level, const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Estimate the size of the full message
    int size = snprintf(NULL, 0, "%s %s\n", level, format) + 1; // +1 for the null terminator
    char* full_format = (char*)malloc(size);
    if (full_format != NULL) {
        snprintf(full_format, size, "%s %s\n", level, format);
        vprintf(full_format, args);
        free(full_format);
    }

    va_end(args);
}

#define LOGGING_INFO
#define LOGGING_ERROR

#ifdef LOGGING_INFO
#define LOG_INFO(format, ...) log_message("[INFO]", format, ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif

#ifdef LOGGING_ERROR
#define LOG_ERROR(format, ...) log_message("[ERROR]", format, ##__VA_ARGS__)
#else
#define LOG_ERROR(format, ...)
#endif


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
		LOG_INFO("Creating %s", m_name);

		if(((VKLCreateInfo<T>*)(&createInfo))->validate()) {
			LOG_INFO("Create Info is valid");
			if (m_valid) {
				LOG_INFO("Destroying old");
				destroy();
			}

			LOG_INFO("Calling _create");
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
