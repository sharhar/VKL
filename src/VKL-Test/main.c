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

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = NULL;
		semaphoreCreateInfo.flags = 0;

		device->pvkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &swapChain->presentCompleteSemaphore);
		device->pvkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &swapChain->renderingCompleteSemaphore);

		VkResult result = device->pvkAcquireNextImageKHR(device->device, swapChain->swapChain, UINT64_MAX,
			swapChain->presentCompleteSemaphore, VK_NULL_HANDLE, &swapChain->nextImageIdx);

		VkCommandBufferBeginInfo beginInfo;
		memset(&beginInfo, 0, sizeof(VkCommandBufferBeginInfo));
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		device->pvkBeginCommandBuffer(devCon->setupCmdBuffer, &beginInfo);

		VkImageMemoryBarrier layoutTransitionBarrier;
		memset(&layoutTransitionBarrier, 0, sizeof(VkImageMemoryBarrier));
		layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		layoutTransitionBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		layoutTransitionBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		layoutTransitionBarrier.image = swapChain->presentImages[swapChain->nextImageIdx];
		layoutTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		layoutTransitionBarrier.subresourceRange.baseMipLevel = 0;
		layoutTransitionBarrier.subresourceRange.levelCount = 1;
		layoutTransitionBarrier.subresourceRange.baseArrayLayer = 0;
		layoutTransitionBarrier.subresourceRange.layerCount = 1;

		device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, NULL,
			0, NULL,
			1, &layoutTransitionBarrier);

		VkClearValue clearValue[] = {
			{ 0.25f, 0.45f, 1.0f, 1.0f },
			{ 1.0, 0.0 } };

		VkRenderPassBeginInfo renderPassBeginInfo;
		memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapChain->renderPass;
		renderPassBeginInfo.framebuffer = swapChain->frameBuffers[swapChain->nextImageIdx];
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = swapChain->width;
		renderPassBeginInfo.renderArea.extent.height = swapChain->height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValue;
		device->pvkCmdBeginRenderPass(devCon->setupCmdBuffer, &renderPassBeginInfo,
			VK_SUBPASS_CONTENTS_INLINE);

		//Render

		device->pvkCmdEndRenderPass(devCon->setupCmdBuffer);

		VkImageMemoryBarrier prePresentBarrier;
		memset(&prePresentBarrier, 0, sizeof(VkImageMemoryBarrier));
		prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		prePresentBarrier.subresourceRange.baseMipLevel = 0;
		prePresentBarrier.subresourceRange.levelCount = 1;
		prePresentBarrier.subresourceRange.baseArrayLayer = 0;
		prePresentBarrier.subresourceRange.layerCount = 1;
		prePresentBarrier.image = swapChain->presentImages[swapChain->nextImageIdx];

		device->pvkCmdPipelineBarrier(devCon->setupCmdBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, NULL,
			0, NULL,
			1, &prePresentBarrier);

		device->pvkEndCommandBuffer(devCon->setupCmdBuffer);

		VkPipelineStageFlags waitStageMash = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
		VkSubmitInfo submitInfo;
		memset(&submitInfo, 0, sizeof(VkSubmitInfo));
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &swapChain->presentCompleteSemaphore;
		submitInfo.pWaitDstStageMask = &waitStageMash;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &devCon->setupCmdBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &swapChain->renderingCompleteSemaphore;
		device->pvkQueueSubmit(devCon->queue, 1, &submitInfo, VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo;
		memset(&presentInfo, 0, sizeof(VkPresentInfoKHR));
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &swapChain->renderingCompleteSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapChain->swapChain;
		presentInfo.pImageIndices = &swapChain->nextImageIdx;
		presentInfo.pResults = NULL;
		result = device->pvkQueuePresentKHR(devCon->queue, &presentInfo);

		//vkWaitForFences(device->device, 1, &swapChain->renderingCompleteFence, VK_TRUE, UINT64_MAX);
		//vkResetFences(device->device, 1, &swapChain->renderingCompleteFence);

		device->pvkQueueWaitIdle(devCon->queue);

		device->pvkDestroySemaphore(device->device, swapChain->presentCompleteSemaphore, NULL);
		device->pvkDestroySemaphore(device->device, swapChain->renderingCompleteSemaphore, NULL);
	}

	vklDestroySwapChain(swapChain);
	vklDestroyDevice(device);
	vklDestroyInstance(instance);

	glfwTerminate();
	
	printf("Enter a key to exit:");
	getchar();
	return 0;
}
