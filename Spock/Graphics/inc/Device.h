#pragma once
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "Instance.h"

class Device {
	public:
		Device(Instance* thisInstance);
		~Device();

		VkPhysicalDevice GetPhysicalDevice();
		VkDevice GetLogicalDevice();
		VkSurfaceKHR GetSurface();
		
		VkExtent2D GetSurfaceExtent();

		uint32_t GetGraphicsQueueIndex();

		int FindMemory(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits);

		VkCommandBuffer* GetCommandBuffers();

	private:
		Instance* ThisInstance = nullptr;

		uint32_t DeviceCount = 0;	
		VkPhysicalDevice* DeviceList = nullptr;
		VkPhysicalDevice DeviceVK = {};

		uint32_t GraphicsQueue = -1;

		VkDeviceQueueCreateInfo DeviceQueueCreateInfo = {};
		VkDeviceCreateInfo LogicalDeviceCreateInfo = {};

		VkDevice LogicalDevice = {};
		VkQueue DeviceQueue = {};

		VkCommandPoolCreateInfo GraphicsPoolCreateInfo = {};
		VkCommandPool GraphicsPool = {};
		VkCommandPoolCreateInfo TransferPoolCreateInfo = {};
		VkCommandPool TransferPool = {};

		VkCommandBufferAllocateInfo GraphicsBufferAllocateInfo = {};
		VkCommandBufferAllocateInfo TransferBufferAllocateInfo = {};

		uint32_t GraphicsBufferCount = 3;
		VkCommandBuffer* GraphicsBuffers = nullptr;

		uint32_t TransferBufferCount = 1;
		VkCommandBuffer* TransferBuffers = nullptr;

	friend class Pipeline;
	friend class  View;
};