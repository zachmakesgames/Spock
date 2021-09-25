#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <exception>
#include "Device.h"
#include "Descriptor.h"

class SetLayout
{
	public:
		SetLayout(Device* device, std::vector<Descriptor> descriptors);
		~SetLayout();
		VkDescriptorSetLayout* GetLayout();

	private:
		Device* device;
		std::vector<Descriptor> descriptors;
		VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
		VkDescriptorSetLayout setLayout;

		VkDescriptorSetLayoutBinding* bindings;
};

