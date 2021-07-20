#include <VKL/VKLQueue.h>
#include <VKL/VKLPhysicalDevice.h>
#include <VKL/VKLSurface.h>

VKLQueue::VKLQueue() {
	m_handle = VK_NULL_HANDLE;
	m_device = NULL;
	m_familyIndex = -1;
}

void VKLQueue::init(VKLDevice* device, VkQueue queue, uint32_t familyIndex) {
	m_handle = queue;
	m_device = device;
	m_familyIndex = familyIndex;
}

VkQueue VKLQueue::handle() {
	return m_handle;
}

uint32_t VKLQueue::getFamilyIndex() {
	return m_familyIndex;
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