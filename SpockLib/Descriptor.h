#pragma once
#include <string>
#include <vulkan/vulkan.h>

class Descriptor {
	public:
		Descriptor(std::string name, VkDescriptorType des_type, VkDeviceSize buffer_size, VkShaderStageFlags stage_flags, uint32_t binding, uint32_t count = 1) :
			name(name), descriptorType(des_type), bufferSize(buffer_size), stageFlags(stage_flags), binding(binding), count(count)
		{}

		std::string name;
		VkDescriptorType descriptorType;
		VkDeviceSize bufferSize;
		VkShaderStageFlags stageFlags;
		uint32_t binding;
		uint32_t count;
};