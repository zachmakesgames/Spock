#pragma once
#include "global.h"
#include <string>
#include <vulkan/vulkan.h>

class c_descriptor {
	public:
		c_descriptor(std::string name, VkDescriptorType descriptor_type, VkDeviceSize buffer_size, VkShaderStageFlags stage_flags, uint32_t binding, uint32_t count= 1) :
			m_name(name), m_descriptor_type(descriptor_type), m_buffer_size(buffer_size), m_stage_flags(stage_flags), m_binding(binding), m_count(count)
		{}

		std::string m_name;
		VkDescriptorType m_descriptor_type;
		VkDeviceSize m_buffer_size;
		VkShaderStageFlags m_stage_flags;
		uint32_t m_binding;
		uint32_t m_count;
};