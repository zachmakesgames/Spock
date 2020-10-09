#include "../inc/Buffer.h"
#define DEBUG

Buffer::Buffer(Device* d, int vertexCount, int vertexSize, VkBufferUsageFlagBits usage) :
	Buffer::Buffer(d, vertexCount * vertexSize, usage) {
	this->vertexCount = vertexCount;

}

Buffer::Buffer(Device* d, VkDeviceSize size, VkBufferUsageFlagBits usage) {
	//possible usages:
	/*
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		VK_BUFFER_USAGE_TRANSFER_DST_BIT
		VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT
		VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
		VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT
		VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT
		VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT
		VK_BUFFER_USAGE_RAY_TRACING_BIT_NV
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT
	 */

	this->ThisDevice = d;
	this->size = size;

	//Set up the buffer creation info
	this->createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	this->createInfo.pNext = nullptr;
	this->createInfo.flags = 0;
	this->createInfo.size = size;
	this->createInfo.usage = usage;
	this->createInfo.queueFamilyIndexCount = 0;	//need to sync this up between all other classes
	this->createInfo.pQueueFamilyIndices = nullptr;
	this->createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	//or CONCURRENT if .queueFamilyIndexCount > 0
	
	VkResult result = vkCreateBuffer(this->ThisDevice->GetLogicalDevice(), &(this->createInfo), nullptr,&(this->buffer));
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating a device buffer: " << result << std::endl;
#endif
		throw result;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->ThisDevice->GetLogicalDevice(), this->buffer, &memRequirements);

	this->memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	this->memAllocInfo.pNext = nullptr;
	this->memAllocInfo.allocationSize = memRequirements.size;

	this->memAllocInfo.memoryTypeIndex = this->ThisDevice->FindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memRequirements.memoryTypeBits);//find host visible memory

	result = vkAllocateMemory(this->ThisDevice->GetLogicalDevice(), &(this->memAllocInfo), nullptr, &(this->memory));
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating device memory: " << result << std::endl;
#endif
		throw result;
	}

	result = vkBindBufferMemory(this->ThisDevice->GetLogicalDevice(), this->buffer, this->memory, 0);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error binding device memory: " << result << std::endl;
#endif
		throw result;
	}
}


void Buffer::Fill(void* data, int size) {
	

	void* gpuMemory;
	vkMapMemory(this->ThisDevice->GetLogicalDevice(), this->memory, 0, VK_WHOLE_SIZE, 0, &gpuMemory);

	if (size > this->size) {
		memcpy(gpuMemory, data, this->size);
	}
	else {
		memcpy(gpuMemory, data, size);
	}

	vkUnmapMemory(this->ThisDevice->GetLogicalDevice(), this->memory);
}

void Buffer::Set(void* data, int size, int offset) {
	
	void* gpuMemory;
	vkMapMemory(this->ThisDevice->GetLogicalDevice(), this->memory, offset, VK_WHOLE_SIZE, 0, &gpuMemory);

	if (size + offset > this->size) {
		memcpy(gpuMemory, data, this->size);
	}
	else {
		memcpy(gpuMemory, data, size);
	}

	vkUnmapMemory(this->ThisDevice->GetLogicalDevice(), this->memory);
}