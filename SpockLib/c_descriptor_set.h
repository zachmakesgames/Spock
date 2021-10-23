#pragma once
#include "global.h"
#include <string>
#include <map>
#include "c_buffer.h"
#include "c_device.h"
#include "c_set_layout.h"

class c_descriptor_set {
	public:
		c_descriptor_set(c_device *device, VkDescriptorPool *descriptor_pool, c_set_layout set_layout);
		VkDescriptorSet *get_vk_descriptor_set();
		VkResult fill_buffer(std::string name, void *data, VkDeviceSize size);
		VkResult set_buffer(std::string name, void *data, VkDeviceSize size, VkDeviceSize offset);
	
	private:
		c_device *m_device;
		VkDescriptorPool *m_pool;
		
		c_set_layout m_set_layout;

		VkDescriptorSetAllocateInfo m_set_alloc_info;

		VkDescriptorSet m_descriptor_set;
		VkDescriptorBufferInfo *m_buffer_info;
		VkWriteDescriptorSet *m_write_descriptors;

		std::map<std::string, c_buffer> m_buffers;
};