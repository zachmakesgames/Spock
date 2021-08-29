#pragma once
#include "Instance.h"
#include <exception>
#include <vector>
#include <iostream>

class Device {
	public:
		Device(Instance* instance);
		VkPhysicalDevice* GetPhysDevice();
		VkDevice* GetLogicalDevice();
		int FindMemory(VkMemoryPropertyFlagBits memory_flag_bits, uint32_t  memory_type_bits);
		VkCommandBuffer* GetGfxCmdBuffers(int* outCount);
		VkCommandBuffer* GetTransferCmdBuffer();
		int GetGfxQueueFamilyIndex();
		int GetTransferQueueFamilyIndex();

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
		uint32_t graphicsQueueFamilyIndex;
		uint32_t transferQueueFamilyIndex;
};