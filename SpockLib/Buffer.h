#pragma once
#include "Device.h"

class Buffer {

	///TODO: Define copy constructor!
	public:
		Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlagBits usage);
		void Fill(void* data, size_t size);
		void Set(void* data, size_t size, uint32_t offset);
		void Clear();
		VkBuffer* GetBuffer();

	private:
		Device* device;
		VkBufferUsageFlagBits usageFlags;
		VkBuffer buffer;
		VkDeviceMemory deviceMemory;
		VkDeviceSize size;
		VkBufferCreateInfo bufferCreateInfo;
		VkMemoryAllocateInfo allocInfo;
		VkMemoryRequirements memoryRequirements;

};