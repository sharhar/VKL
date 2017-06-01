#include <VKL/VKL.h>
#include <stdlib.h>
#include <memory.h>

char* readFileFromPath(char *filename, size_t* size) {
	char *buffer = NULL;
	size_t string_size, read_size;
	FILE *handler = fopen(filename, "rb");

	if (handler) {
		fseek(handler, 0, SEEK_END);
		string_size = ftell(handler);
		rewind(handler);

		buffer = (char*)malloc_c(sizeof(char) * (string_size + 1));

		read_size = fread(buffer, sizeof(char), string_size, handler);

		buffer[string_size] = '\0';

		if (string_size != read_size) {
			printf("Error occured while reading file!\nstring_size = %zu\nread_size = %zu\n\n", string_size, read_size);
			free_c(buffer);
			buffer = NULL;
		}

		*size = read_size;

		fclose(handler);
	} else {
		printf("Did not find file!\n");
	}

	return buffer;
}

void VLKCheck(VkResult result, char *msg) {
	if(result != VK_SUCCESS) {
		printf("%s\n", msg);
	}
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
