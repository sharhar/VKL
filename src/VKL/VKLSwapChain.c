#include <VKL/VKL.h>

int vklCreateSwapChain(VKLDeviceGraphicsContext* context, VKLSwapChain** pSwapChain) {
	VKLSwapChain* swapChain = (VKLSwapChain*)malloc_c(sizeof(VKLSwapChain));



	return 0;
}

int vklDestroySwapChain(VKLSwapChain* swapChain) {


	free_c(swapChain);
	return 0;
}