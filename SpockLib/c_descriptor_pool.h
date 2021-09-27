#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include "c_device.h"
#include "c_descriptor_set.h"
#include "c_set_layout.h"


class c_descriptor_pool {
	public:
		c_descriptor_pool(c_device *device, VkDescriptorPoolSize *pool_sizes, uint32_t pool_size_count);
		c_descriptor_set produce_descriptor_set(c_set_layout set_layout);

	private:
		c_device *m_device;
		VkDescriptorPoolSize *m_pool_sizes;
		uint32_t m_pool_size_count;

		VkDescriptorPool m_descriptor_pool;
		VkDescriptorPoolCreateInfo m_pool_create_info;
};