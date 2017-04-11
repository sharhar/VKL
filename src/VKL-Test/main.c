#include <VKL/VKL.h>

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VKL Test", NULL, NULL);

	uint8_t debug = 0;

#ifdef _DEBUG
	debug = 1;
#endif

	VKLInstance* instance;
	vklCreateInstance(&instance, debug);
	
	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;
	vklCreateDevice(instance, &device, &window, 1, &deviceContexts, 0, NULL);

	VKLDeviceGraphicsContext* devCon = deviceContexts[0];
	
	VKLSwapChain* swapChain;
	vklCreateSwapChain(devCon, &swapChain);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	vklDestroyDevice(device);
	vklDestroyInstance(instance);

	glfwTerminate();
	
	printf("Enter a key to exit:");
	getchar();
	return 0;
}
