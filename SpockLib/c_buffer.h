#pragma once
#include "global.h"
#include "c_device.h"

class c_buffer {

	///TODO: Define copy constructor!
	public:
		c_buffer(c_device *device, VkDeviceSize size, VkBufferUsageFlagBits usage);
		void fill(void *data, size_t size);
		void set(void *data, size_t size, uint32_t offset);
		void clear();
		VkBuffer* get_buffer();

	private:
		c_device *m_device;
		VkBufferUsageFlagBits m_usage_flags;
		VkBuffer m_buffer;
		VkDeviceMemory m_device_memory;
		VkDeviceSize m_size;
		VkBufferCreateInfo m_buffer_create_info;
		VkMemoryAllocateInfo m_alloc_info;
		VkMemoryRequirements m_memory_requirements;

};