#include <VKL/VKL.h>
#include <GLFW/glfw3.h>
#include <math.h>

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "VKL Test", NULL, NULL);
	
	VkBool32 debug = 0;
	
#ifdef _DEBUG
	debug = 1;
#endif

	VKLInstance* instance;
	vklCreateInstance(&instance, NULL, debug, NULL);
	
	VKLSurface* surface;
	vklCreateGLFWSurface(instance, &surface, window);

	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;
	vklCreateDevice(instance, &device, &surface, 1, &deviceContexts, 0, NULL);

	VKLDeviceGraphicsContext* devCon = deviceContexts[0];
	
	VKLSwapChain* swapChain;
	vklCreateSwapChain(devCon, &swapChain, VK_TRUE);

	float data[] = {
		-0.8f, -0.8f,
		 0.0f,  0.8f,
		 0.8f, -0.8f
	};
	
	VKLBuffer* buffer;
	vklCreateStagedBuffer(devCon, &buffer, data, sizeof(float) * 6, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	char* shaderPaths[2];
	shaderPaths[0] = "res/basic-vert.spv";
	shaderPaths[1] = "res/basic-frag.spv";

	VkShaderStageFlagBits stages[2];
	stages[0] = VK_SHADER_STAGE_VERTEX_BIT;
	stages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;

	size_t offset = 0;
	VkFormat format = VK_FORMAT_R32G32_SFLOAT;

	VKLShaderCreateInfo shaderCreateInfo;
	memset(&shaderCreateInfo, 0, sizeof(VKLShaderCreateInfo));
	shaderCreateInfo.shaderPaths = shaderPaths;
	shaderCreateInfo.shaderStages = stages;
	shaderCreateInfo.shaderCount = 2;
	shaderCreateInfo.vertexInputAttributeStride = sizeof(float) * 2;
	shaderCreateInfo.vertexInputAttributesCount = 1;
	shaderCreateInfo.vertexInputAttributeOffsets = &offset;
	shaderCreateInfo.vertexInputAttributeFormats = &format;

	VKLShader* shader;
	vklCreateShader(device, &shader, &shaderCreateInfo);

	VKLPipelineCreateInfo pipelineCreateInfo;
	memset(&pipelineCreateInfo, 0, sizeof(VKLPipelineCreateInfo));
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = swapChain->renderPass;
	pipelineCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineCreateInfo.extent.width = swapChain->width;
	pipelineCreateInfo.extent.height = swapChain->height;

	VKLGraphicsPipeline* pipeline;
	vklCreateGraphicsPipeline(device, &pipeline, &pipelineCreateInfo);
	
	VkCommandBuffer cmdBuffer;
	vklAllocateCommandBuffer(devCon, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	vklSetClearColor(swapChain, 0.25f, 0.45f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		vklClearScreen(swapChain);

		vklBeginRenderRecording(swapChain, cmdBuffer);
		
		VkViewport viewport = { 0, 0, swapChain->width, swapChain->height, 0, 1 };
		VkRect2D scissor = { 0, 0, swapChain->width, swapChain->height };
		VkDeviceSize offsets = 0;

		device->pvkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		device->pvkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
		
		device->pvkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		device->pvkCmdBindVertexBuffers(cmdBuffer, 0, 1, &buffer->buffer, &offsets);

		device->pvkCmdDraw(cmdBuffer, 3, 1, 0, 0);
		
		vklEndRenderRecording(swapChain, cmdBuffer);

		vklRenderRecording(swapChain, cmdBuffer);

		vklSwapBuffers(swapChain);
	}

	vklDestroyGraphicsPipeline(device, pipeline);
	vklDestroyShader(device, shader);
	vklDestroyBuffer(device, buffer);
	vklDestroySwapChain(swapChain);
	vklDestroyDevice(device);
	vklDestroySurface(instance, surface);
	vklDestroyInstance(instance);

	glfwTerminate();
	
	printf("Enter a key to exit:");
	getchar();
	return 0;
}
