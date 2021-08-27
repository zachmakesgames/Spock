#pragma once
#include "Instance.h"
#include <exception>
#include <vector>

class Device {
	public:
		Device(Instance* instance);
		VkPhysicalDevice* GetPhysDevice();
		VkDevice* GetLogicalDevice();
		int FindMemory(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits);
		VkCommandBuffer* GetGfxCmdBuffers(int* outCount);
		VkCommandBuffer* GetTransferCmdBuffer();

	private:
		Instance* instance; 
		VkPhysicalDevice physDevice;
		VkDevice logicalDevice;
		VkQueue queue;
		VkCommandPool gfxCmdPool;
		VkCommandPool transferCmdPool;
		VkCommandBuffer* gfxCmdBuffers;
		VkCommandBuffer* transferCmdBuffer;
		VkDeviceQueueCreateInfo deviceQueueCreateInfo;
		VkDeviceCreateInfo deviceCreateInfo;
		VkCommandPoolCreateInfo gfxPoolCreateInfo;
		VkCommandPoolCreateInfo transferCreateInfo;

		VkCommandBufferAllocateInfo gfxBufferAllocInfo;
		VkCommandBufferAllocateInfo transferBufferAllocInfo;
		uint32_t graphicsBufferCount = 3;
		uint32_t transferBufferCount = 1;
		
};