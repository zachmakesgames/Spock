#pragma once
#include "global.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <exception>
#include "c_device.h"
#include "c_descriptor.h"

class c_set_layout
{
	public:
		c_set_layout(c_device *device, std::vector<c_descriptor> descriptors);
		~c_set_layout();
		VkDescriptorSetLayout *get_layout();

	private:
		c_device *m_device;
		std::vector<c_descriptor> m_descriptors;
		VkDescriptorSetLayoutCreateInfo m_layout_create_info;
		VkDescriptorSetLayout m_set_layout;

		VkDescriptorSetLayoutBinding *m_bindings;
};

