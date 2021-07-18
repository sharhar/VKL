#include <VKL/VKlInstance.h>
#include <VKL/VKLSurface.h>
#include <VKL/VKLQueue.h>
#include <VKL/VKLDevice.h>
#include <VKL/VKLPhysicalDevice.h>

#define GLFW_INCLUDE_NONE
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

	uint32_t glfwExtensionCount = 0;
	char** glfwExtensions = (char**)glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VKLInstanceOptions instanceOptions;
	instanceOptions.addExtensions(glfwExtensions, glfwExtensionCount);
	instanceOptions.setDebug(debug);

	VKLInstance instance(glfwGetInstanceProcAddress, &instanceOptions);

	VkSurfaceKHR glfwSurface = VK_NULL_HANDLE;
	glfwCreateWindowSurface(instance.handle(), window, NULL, &glfwSurface);

	VKLSurface surface(glfwSurface, &instance);
	surface.setSize(800, 600);

	VKLPhysicalDevice physicalDevice = *instance.getPhysicalDevices()[0];

	for (int i = 0; i < physicalDevice.getQueueFamilyProperties().size(); i++) {
		printf("%d: %d (%d)\n", i, physicalDevice.getQueueFamilyProperties()[i].queueFlags, physicalDevice.getQueueFamilyProperties()[i].queueCount);
	}

	VKLQueueCreateInfo queueCreateInfo(3, &physicalDevice);

	queueCreateInfo.setQueueType(0, 1, queueCreateInfo.getQueueFamilyIndexWithSurfaceSupport(VK_QUEUE_GRAPHICS_BIT, 0, surface));
	queueCreateInfo.setQueueType(1, 1, queueCreateInfo.getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT));
	queueCreateInfo.setQueueType(2, 1, queueCreateInfo.getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT));

	VKLDeviceCreateInfo deviceCreateInfo(&instance, &physicalDevice, &queueCreateInfo);
	deviceCreateInfo.addExtension("VK_KHR_swapchain");

	VKLDevice device(&deviceCreateInfo);

	VKLQueue graphicsQueue = device.getQueue(0, 0);
	VKLQueue computeQueue =  device.getQueue(1, 0);
	VKLQueue transferQueue = device.getQueue(2, 0);

	/*
	
	VKLSwapChain* swapChain;
	VKLFrameBuffer* backBuffer;
	vklCreateSwapChain(devCon, &swapChain, VK_TRUE);
	vklGetBackBuffer(swapChain, &backBuffer);

	float data[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,

		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f,
	};
	
	VKLBuffer* buffer;
	vklCreateStagedBuffer(devCon, &buffer, data, sizeof(float) * 6 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	char* shaderPaths[2];
	shaderPaths[0] = "res/basic-vert.spv";
	shaderPaths[1] = "res/basic-frag.spv";

	VkShaderStageFlagBits stages[2];
	stages[0] = VK_SHADER_STAGE_VERTEX_BIT;
	stages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;

	size_t offsets[2] = {0, sizeof(float) * 2};
	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };

	VkDescriptorSetLayoutBinding bindings[2];
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = NULL;

	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[1].pImmutableSamplers = NULL;

	uint32_t vertexInputAttributeBindings[] = { 0 , 0 };

	VKLShaderCreateInfo shaderCreateInfo;
	memset(&shaderCreateInfo, 0, sizeof(VKLShaderCreateInfo));
	shaderCreateInfo.shaderPaths = shaderPaths;
	shaderCreateInfo.shaderStages = stages;
	shaderCreateInfo.shaderCount = 2;
	shaderCreateInfo.bindings = bindings;
	shaderCreateInfo.bindingsCount = 2;
	shaderCreateInfo.vertexInputAttributesCount = 2;
	shaderCreateInfo.vertexInputAttributeOffsets = offsets;
	shaderCreateInfo.vertexInputAttributeFormats = formats;
	shaderCreateInfo.vertexInputAttributeBindings = vertexInputAttributeBindings;

	VkVertexInputRate vertInputRate[] = { VK_VERTEX_INPUT_RATE_VERTEX };
	size_t stride[] = { sizeof(float) * 4 };
	shaderCreateInfo.vertexBindingsCount = 1;
	shaderCreateInfo.vertexBindingInputRates = vertInputRate;
	shaderCreateInfo.vertexBindingStrides = stride;

	VKLShader* shader;
	vklCreateShader(device, &shader, &shaderCreateInfo);

	VKLGraphicsPipelineCreateInfo pipelineCreateInfo;
	memset(&pipelineCreateInfo, 0, sizeof(VKLGraphicsPipelineCreateInfo));
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = backBuffer->renderPass;
	pipelineCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineCreateInfo.extent.width = swapChain->width;
	pipelineCreateInfo.extent.height = swapChain->height;

	VKLPipeline* pipeline;
	vklCreateGraphicsPipeline(device, &pipeline, &pipelineCreateInfo);
	
	VkCommandBuffer cmdBuffer;
	vklAllocateCommandBuffer(devCon, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	VKLUniformObject* uniform;
	vklCreateUniformObject(device, &uniform, shader);

	float proj[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	VKLBuffer* uniformBuffer;
	vklCreateBuffer(device, &uniformBuffer, VK_FALSE, sizeof(float) * 16, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	
	vklSetUniformBuffer(device, uniform, uniformBuffer, 0);

	float imageData[] = {
		1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,
		1.0f, 0.3f, 0.3f,    0.3f, 1.0f, 0.3f,   0.3f, 0.3f, 1.0f,    0.3f, 0.3f, 0.3f,
		1.0f, 0.5f, 0.5f,    0.5f, 1.0f, 0.5f,   0.5f, 0.5f, 1.0f,    0.5f, 0.5f, 0.5f,
		1.0f, 0.8f, 0.8f,    0.8f, 1.0f, 0.8f,   0.8f, 0.8f, 1.0f,    0.8f, 0.8f, 0.8f
	};

	VKLTextureCreateInfo textureCreateInfo;
	textureCreateInfo.width = 4;
	textureCreateInfo.height = 4;
	textureCreateInfo.format = VK_FORMAT_R32G32B32_SFLOAT;
	textureCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	textureCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	textureCreateInfo.filter = VK_FILTER_NEAREST;
	textureCreateInfo.colorSize = sizeof(float);
	textureCreateInfo.colorCount = 3;

	VKLTexture* texture;
	vklCreateStagedTexture(devCon, &texture, &textureCreateInfo, imageData);

	vklSetUniformTexture(device, uniform, texture, 1);

	vklSetClearColor(backBuffer, 0.25f, 0.45f, 1.0f, 1.0f);

	{ //Record rendering commands
		vklBeginCommandBuffer(device, cmdBuffer);

		vklBeginRender(device, backBuffer, cmdBuffer);

		VkViewport viewport = { 0, 0, swapChain->width, swapChain->height, 0, 1 };
		VkRect2D scissor = { 0, 0, swapChain->width, swapChain->height };
		VkDeviceSize vertexOffsets = 0;

		device->pvkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		device->pvkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		device->pvkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		device->pvkCmdBindVertexBuffers(cmdBuffer, 0, 1, &buffer->buffer, &vertexOffsets);

		device->pvkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline->pipelineLayout, 0, 1, &uniform->descriptorSet, 0, NULL);

		device->pvkCmdDraw(cmdBuffer, 6, 1, 0, 0);

		vklEndRender(device, backBuffer, cmdBuffer);

		vklEndCommandBuffer(device, cmdBuffer);
	}

	*/

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		/*
		proj[12] = glfwGetTime() / 10.0f;

		vklWriteToMemory(device, uniformBuffer->memory, proj, sizeof(float) * 16, 0);
		
		vklExecuteCommandBuffer(devCon, cmdBuffer);

		vklPresent(swapChain);
		*/
	}

	/*
	vklDestroyTexture(device, texture);
	vklDestroyUniformObject(device, uniform);
	vklDestroyPipeline(device, pipeline);
	vklDestroyShader(device, shader);
	vklDestroyBuffer(device, uniformBuffer);
	vklDestroyBuffer(device, buffer);
	vklDestroySwapChain(swapChain);
	*/

	device.destroy();
	surface.destroy();
	instance.destroy();

	glfwTerminate();
	return 0;
}
