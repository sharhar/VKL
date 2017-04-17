#include <VKL/VKL.h>
#include <malloc.h>

void VLKCheck(VkResult result, char *msg) {
	assert(result == VK_SUCCESS, msg);
}

void* malloc_c(size_t size) {
	void* result = malloc(size);
	memset(result, 0, size);
	return result;
}

void free_c(void* block) {
	free(block);
}

void* realloc_c(void* pOriginal, size_t size) {
	return realloc(pOriginal, size);
}