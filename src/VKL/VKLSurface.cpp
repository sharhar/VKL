#include <VKL/VKLSurface.h>
#include <VKL/VKLInstance.h>

VKLSurface::VKLSurface(VkSurfaceKHR surface, VKLInstance& instance) : m_instance(instance) {
	m_surface = VK_NULL_HANDLE;
	m_width = -1;
	m_height = -1;
	m_surface = surface;
}

VkSurfaceKHR VKLSurface::handle() {
	return m_surface;
}

void VKLSurface::setSize(int width, int height) {
	m_width = width;
	m_height = height;
}

void VKLSurface::destroy() {
	m_instance.vk.DestroySurfaceKHR(m_instance.handle(), m_surface, m_instance.allocator());
}