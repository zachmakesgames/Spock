#pragma once
#include "vulkan/vulkan.h"
#include "Device.h"
#include "Buffer.h"
#include "../../Utilities/HashTable.h"
#include "ShaderPack.h"

class DescriptorSet {
	public:
		DescriptorSet(Device* d, VkDescriptorPool* pool, ShaderPack* pack);
		VkDescriptorSet* GetDescriptorSetVK();
		VkDescriptorSetLayout* GetDescriptorSetLayout();


		//void Fill(void* data, int size);
		//void Set(void* data, int size, int offset);
		VkResult FillUniform(std::string uniformName, void* data, int size);
		VkResult SetUniform(std::string uniformName, void* data, int size, int offset);

		uint64_t GetDescriptorSetCount();
	private:
		VkResult result;

		ShaderPack* ThisPack = nullptr;

		Device* ThisDevice = nullptr;
		VkDescriptorPool* ThisPool = nullptr;

		VkDescriptorSetLayoutCreateInfo* SetLayoutCreateInfo = nullptr;

		VkDescriptorSetLayout* SetLayouts = nullptr;
		
		VkDescriptorSetAllocateInfo SetAllocateInfo = {};

		VkDescriptorSet* DescriptorSetVK = nullptr;

		VkDescriptorBufferInfo* BufferInfo = nullptr;

		VkWriteDescriptorSet* WriteDescriptors = nullptr;

		HashTable<Buffer> UniformBuffers;
		uint64_t LayoutCount;


};