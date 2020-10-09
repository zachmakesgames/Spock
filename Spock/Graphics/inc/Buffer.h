#pragma once
#include "vulkan/vulkan.h"
#include "../inc/Device.h"



class Buffer {
	private:
		VkBuffer		buffer = {};
		VkDeviceMemory		memory = {};
		VkDeviceSize		size = {};
		
		VkBufferCreateInfo createInfo = {};
		VkMemoryAllocateInfo memAllocInfo = {};

		Device* ThisDevice = nullptr;
		VkDevice* ThisVkDevice = nullptr;
		VkPhysicalDevice* ThisPDevice = nullptr;

		uint64_t vertexCount = 0;

	public:
		Buffer(Device* d, VkDeviceSize size, VkBufferUsageFlagBits usage);
		Buffer(Device* d, int vertexCount, int vertexSize, VkBufferUsageFlagBits usage);
		//Buffer(VkDevice* d, VkPhysicalDevice* pd, VkDeviceSize size, VkBufferUsageFlagBits usage);
		void Fill(void* data, int size);
		void Set(void* data, int size, int offset);

		inline VkBuffer VkBuffer() {
			return this->buffer;
		}

		inline uint64_t VertexCount() {
			return this->vertexCount;
		}


	friend class Pipeline;
};