#pragma once
#include <string>
#include <map>
#include "Buffer.h"
#include "Device.h"
#include "SetLayout.h"

class DescriptorSet {
	public:
		DescriptorSet(Device* device, VkDescriptorPool* descriptor_pool, SetLayout set_layout);
		VkDescriptorSet* GetVkDescriptorSet();
		VkResult FillBuffer(std::string name, void* data, VkDeviceSize size);
		VkResult SetBuffer(std::string name, void* data, VkDeviceSize size, VkDeviceSize offset);
	
	private:
		Device* device;
		VkDescriptorPool* pool;
		
		SetLayout setLayout;

		VkDescriptorSetAllocateInfo setAllocInfo;

		VkDescriptorSet descriptorSet;
		VkDescriptorBufferInfo* bufferInfo;
		VkWriteDescriptorSet* writeDescriptors;

		std::map<std::string, Buffer> buffers;
};