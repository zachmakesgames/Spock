#include "Buffer.h"

Buffer::Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlagBits usage) :device(device), size(size), usageFlags(usage) {
	

	VkResult result;

	//Set up the buffer creation info
	this->bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	this->bufferCreateInfo.pNext = nullptr;
	this->bufferCreateInfo.flags = 0;
	this->bufferCreateInfo.size = this->size;
	this->bufferCreateInfo.usage = this->usageFlags;
	this->bufferCreateInfo.queueFamilyIndexCount = this->device->GetGfxQueueFamilyIndex();
	this->bufferCreateInfo.pQueueFamilyIndices = nullptr;
	this->bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	//or CONCURRENT if .queueFamilyIndexCount > 0


	result = vkCreateBuffer(*this->device->GetLogicalDevice(), &this->bufferCreateInfo, nullptr, &this->buffer);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create buffer: " << result << std::endl;
#endif
		throw std::exception("Could not create buffer");
	}

	
	//Get memory requirements
	vkGetBufferMemoryRequirements(*this->device->GetLogicalDevice(), this->buffer, &this->memoryRequirements);

	//Set up memory allocate info
	this->allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	this->allocInfo.pNext = nullptr;
	this->allocInfo.allocationSize = this->memoryRequirements.size;
	this->allocInfo.memoryTypeIndex = this->device->FindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, this->memoryRequirements.memoryTypeBits);

	this->deviceMemory = {};

	//Allocate memory
	result = vkAllocateMemory(*this->device->GetLogicalDevice(), &this->allocInfo, nullptr, &this->deviceMemory);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not allocate device memory: " << result << std::endl;
#endif
		throw std::exception("Could not allocate device memory");
	}


	//Bind the memory
	result = vkBindBufferMemory(*this->device->GetLogicalDevice(), this->buffer, this->deviceMemory, 0);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not bind device memory: " << result << std::endl;
#endif
		throw std::exception("Could not bind device memory");
	}

}


void Buffer::Fill(void* data, size_t size) {
	void* gpu_memory;
	vkMapMemory(*this->device->GetLogicalDevice(), this->deviceMemory, 0, VK_WHOLE_SIZE, 0, &gpu_memory);

	if (size > this->size) {
		memcpy(gpu_memory, data, this->size);
	}
	else {
		memcpy(gpu_memory, data, size);
	}

	vkUnmapMemory(*this->device->GetLogicalDevice(), this->deviceMemory);
}

void Buffer::Set(void* data, size_t size, uint32_t offset) {
	void* gpu_memory;
	vkMapMemory(*this->device->GetLogicalDevice(), this->deviceMemory, offset, VK_WHOLE_SIZE, 0, &gpu_memory);

	if (size + offset > this->size) {
		memcpy(gpu_memory, data, this->size);
	}
	else {
		memcpy(gpu_memory, data, size);
	}

	vkUnmapMemory(*this->device->GetLogicalDevice(), this->deviceMemory);
}

void Buffer::Clear() {
	void* gpu_memory;
	vkMapMemory(*this->device->GetLogicalDevice(), this->deviceMemory, 0, VK_WHOLE_SIZE, 0, &gpu_memory);

	memset(gpu_memory, 0x00, this->size);

	vkUnmapMemory(*this->device->GetLogicalDevice(), this->deviceMemory);
}

VkBuffer* Buffer::GetBuffer() {
	return &this->buffer;
}