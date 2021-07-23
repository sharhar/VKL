#include <VKL/VKL.h>

VKLQueue::VKLQueue() {
	m_handle = VK_NULL_HANDLE;
	m_device = NULL;
	m_familyIndex = -1;
}

void VKLQueue::init(VKLDevice* device, VkQueue queue, uint32_t familyIndex) {
	m_handle = queue;
	m_device = device;
	m_familyIndex = familyIndex;
	
	m_cmdBuffer = new VKLCommandBuffer(this);
}

VkQueue VKLQueue::handle() {
	return m_handle;
}

VKLDevice* VKLQueue::getDevice() {
	return m_device;
}

VKLCommandBuffer* VKLQueue::getCmdBuffer() {
	return m_cmdBuffer;
}

uint32_t VKLQueue::getFamilyIndex() {
	return m_familyIndex;
}

void VKLQueue::submit(VKLCommandBuffer* cmdBuffer, VkFence fence) {
	VkCommandBuffer cmdBuffHandle = cmdBuffer->handle();
	
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(VkSubmitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffHandle;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
	
	VK_CALL(m_device->vk.QueueSubmit(m_handle, 1, &submitInfo, fence));
}

void VKLQueue::waitIdle() {
	m_device->vk.QueueWaitIdle(m_handle);
}

VKLQueueCreateInfo::VKLQueueCreateInfo(uint32_t typeCount, VKLPhysicalDevice* physicalDevice) {
	priorities = (float**)malloc(sizeof(float*) * typeCount);
	this->typeCount = typeCount;

	this->physicalDevice = physicalDevice;
	
	for (int i = 0; i < typeCount; i++) {
		priorities[i] = NULL;
	}

	createInfos = (VkDeviceQueueCreateInfo*)malloc(sizeof(VkDeviceQueueCreateInfo) * typeCount);
	memset(createInfos, 0, sizeof(VkDeviceQueueCreateInfo) * typeCount);
}

int32_t VKLQueueCreateInfo::getQueueFamilyIndex(VkQueueFlags supportFlags, VkQueueFlags excludeFlags) {
	for (int i = 0; i < physicalDevice->getQueueFamilyProperties().size(); i++) {
		VkQueueFlags queueFlags = physicalDevice->getQueueFamilyProperties()[i].queueFlags;
		if ((queueFlags & supportFlags) && !(queueFlags & excludeFlags)) {
			return i;
		}
	}

	return -1;
}

int32_t VKLQueueCreateInfo::getQueueFamilyIndexWithSurfaceSupport(VkQueueFlags supportFlags, VkQueueFlags excludeFlags, VKLSurface& surface) {
	for (int i = 0; i < physicalDevice->getQueueFamilyProperties().size(); i++) {
		VkQueueFlags queueFlags = physicalDevice->getQueueFamilyProperties()[i].queueFlags;
		if ((queueFlags & supportFlags) && !(queueFlags & excludeFlags) && surface.getPhysicalDeviceSupport(physicalDevice, i)) {
			return i;
		}
	}

	return -1;
}

void VKLQueueCreateInfo::setQueueType(uint32_t typeIndex, uint32_t count, uint32_t queueFamilyIndex) {
	priorities[typeIndex] = (float*)malloc(sizeof(float) * count);

	for (int i = 0; i < count; i++) {
		priorities[typeIndex][i] = 1.0f;
	}
	
	createInfos[typeIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfos[typeIndex].pNext = NULL;
	createInfos[typeIndex].flags = 0;
	createInfos[typeIndex].queueCount = count;
	createInfos[typeIndex].pQueuePriorities = priorities[typeIndex];
	createInfos[typeIndex].queueFamilyIndex = queueFamilyIndex;
}

void VKLQueueCreateInfo::setQueuePriorities(uint32_t typeIndex, float* priorityList) {
	memcpy(priorities[typeIndex], priorityList, sizeof(float) * createInfos[typeIndex].queueCount);
}

VKLQueueCreateInfo::~VKLQueueCreateInfo() {
	for (int i = 0; i < typeCount; i++) {
		if (priorities[i] != NULL) {
			free(priorities[i]);
		}
	}

	free(priorities);
	free(createInfos);
}
