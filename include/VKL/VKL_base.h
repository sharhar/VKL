#ifndef VKL_base_h
#define VKL_base_h

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct VKLInstanceOptions;
typedef struct VKLQueueCreateInfo;
typedef struct VKLDeviceCreateInfo;
typedef struct VKLImageCreateInfo;

class VKLInstance;
class VKLSurface;
class VKLPhysicalDevice;
class VKLDevice;
class VKLQueue;
class VKLImage;
class VKLSwapChain;

#ifdef _DEBUG
#define VK_CALL(result) {VkResult ___result = result; if(___result == VK_SUCCESS) { printf("(VkResult = %d) " #result " in " __FUNCTION__ " in " __FILE__ "\n", ___result); }}
#endif

#ifndef _DEBUG
#define VK_CALL(result) result;
#endif


#endif
