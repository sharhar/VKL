#include <VKL/VKL.h>
#include <malloc.h>

void VLKCheck(VkResult result, char *msg) {
	assert(result == VK_SUCCESS, msg);
}

void* malloc_c(size_t size) {
	return malloc(size);
}

void free_c(void* block) {
	free(block);
}