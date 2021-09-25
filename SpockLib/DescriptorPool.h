#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include "Device.h"
#include "DescriptorSet.h"
#include "SetLayout.h"


class DescriptorPool {
	public:
		DescriptorPool(Device* device, VkDescriptorPoolSize* pool_sizes, uint32_t pool_size_count);
		DescriptorSet ProduceDescriptorSet(SetLayout set_layout);

	private:
		Device* device;
		VkDescriptorPoolSize* poolSizes;
		uint32_t poolSizeCount;

		VkDescriptorPool descriptorPool;
		VkDescriptorPoolCreateInfo poolCreateInfo;
};