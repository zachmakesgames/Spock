#pragma once
#include "c_instance.h"
#include <exception>
#include <vector>
#include <iostream>

class c_device {
	public:
		c_device(c_instance *instance);
		VkPhysicalDevice *get_physical_device();
		VkDevice *get_logical_device();
		int find_memory(VkMemoryPropertyFlagBits memory_flag_bits, uint32_t  memory_type_bits);
		VkCommandBuffer *get_graphics_command_buffers(int *out_count);
		VkCommandBuffer *get_transfer_command_buffers(int * out_count);
		int get_graphics_queue_family_index();
		int get_transfer_queue_family_index();

	private:
		c_instance *m_instance; 
		VkPhysicalDevice m_phys_device;
		VkDevice m_logical_device;
		VkQueue m_queue;
		VkCommandPool m_graphics_command_pool;
		VkCommandPool m_transfer_command_pool;
		VkCommandBuffer *m_graphics_command_buffer;
		VkCommandBuffer *m_transfer_command_buffer;
		VkDeviceQueueCreateInfo m_device_queue_create_info;
		VkDeviceCreateInfo m_device_create_info;
		VkCommandPoolCreateInfo m_graphics_pool_create_info;
		VkCommandPoolCreateInfo m_transfer_pool_create_info;

		VkCommandBufferAllocateInfo m_graphics_buffer_alloc_info;
		VkCommandBufferAllocateInfo m_transfer_buffer_alloc_info;

		uint32_t m_graphics_buffer_count= 3;
		uint32_t m_transfer_buffer_count= 1;
		uint32_t m_graphics_queue_familyindex= 0;
		uint32_t m_transfer_queue_family_index= 0;
};