#ifndef VKLInstance_h
#define VKLInstance_h

#include "VKL_base.h"

#include <vector>

typedef struct VKLInstanceOptions {

	VKLInstanceOptions();

	void setAllocator(VkAllocationCallbacks* allocator);
	void addExtension(char* extension);
	void addExtensions(char** extensions, uint32_t extensionCount);
	void addExtensions(std::vector<char*> extensions);
	void setDebug(VkBool32 debug);

	VkAllocationCallbacks* m_allocator;
	std::vector<char*> m_extensions;
	VkBool32 m_debug;

} VKLInstanceOptions;

class VKLInstance {
private:
	VkInstance m_instance;

	VkAllocationCallbacks* m_allocator;

	VkDebugReportCallbackEXT m_debugCallback;

	std::vector<char*> m_layers;
	std::vector<char*> m_extensions;

	VkBool32 m_debug;
public:
	VKLInstance();
	VKLInstance(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options);

	void create(PFN_vkGetInstanceProcAddr vkFunct, VKLInstanceOptions* options);
	VkAllocationCallbacks* allocator();
	VkInstance handle();
	void destroy();

#define FUNC_DESC
#include "VKLInstance_autogen.h"
#undef FUNC_DESC
	
private:																																														

#define PFNS
#include "VKLInstance_autogen.h"
#undef PFNS
};

#endif