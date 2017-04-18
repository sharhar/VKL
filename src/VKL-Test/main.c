#include <VKL/VKL.h>

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VKL Test", NULL, NULL);

	VkBool32 debug = 0;
	
#ifdef _DEBUG
	debug = 1;
#endif

	VKLInstance* instance;
	vklCreateInstance(&instance, NULL, debug);
	
	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;
	vklCreateDevice(instance, &device, &window, 1, &deviceContexts, 0, NULL);

	VKLDeviceGraphicsContext* devCon = deviceContexts[0];
	
	VKLSwapChain* swapChain;
	vklCreateSwapChain(devCon, &swapChain, VK_TRUE);

	vklSetClearColor(swapChain, 0.25f, 0.45f, 1.0f, 1.0f);

	VkCommandBuffer* cmdBuffers = malloc_c(sizeof(VkCommandBuffer) * swapChain->imageCount);
	vklAllocateCommandBuffer(devCon, cmdBuffers, VK_COMMAND_BUFFER_LEVEL_PRIMARY, swapChain->imageCount);

	for (int i = 0; i < swapChain->imageCount;i++) {
		vklClearScreen(swapChain);

		vklBeginRenderRecording(swapChain, cmdBuffers[i]);
		vklEndRenderRecording(swapChain, cmdBuffers[i]);

		vklRenderRecording(swapChain, cmdBuffers[i]);

		vklSwapBuffers(swapChain);
	}

	float data[] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	VKLBuffer* buffer;
	vklCreateStagedBuffer(devCon, &buffer, data, sizeof(float) * 6, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		vklClearScreen(swapChain);

		vklRenderRecording(swapChain, cmdBuffers[swapChain->nextImageIdx]);

		vklSwapBuffers(swapChain);
	}

	vklDestroyBuffer(device, buffer);
	vklDestroySwapChain(swapChain);
	vklDestroyDevice(device);
	vklDestroyInstance(instance);

	glfwTerminate();
	
	printf("Enter a key to exit:");
	getchar();
	return 0;
}
