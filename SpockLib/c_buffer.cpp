#include "c_buffer.h"

c_buffer::c_buffer(c_device *device, VkDeviceSize size, VkBufferUsageFlagBits usage) :m_device(device), m_size(size), m_usage_flags(usage) {
	

	VkResult result;

	//Set up the buffer creation info
	this->m_buffer_create_info.sType= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	this->m_buffer_create_info.pNext= nullptr;
	this->m_buffer_create_info.flags= 0;
	this->m_buffer_create_info.size= this->m_size;
	this->m_buffer_create_info.usage= this->m_usage_flags;
	this->m_buffer_create_info.queueFamilyIndexCount= this->m_device->get_graphics_queue_family_index();
	this->m_buffer_create_info.pQueueFamilyIndices= nullptr;
	this->m_buffer_create_info.sharingMode= VK_SHARING_MODE_EXCLUSIVE;	//or CONCURRENT if .queueFamilyIndexCount > 0


	result = vkCreateBuffer(*this->m_device->get_logical_device(), &this->m_buffer_create_info, nullptr, &this->m_buffer);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create buffer: " << result << std::endl;
#endif
		throw std::exception("Could not create buffer");
	}

	
	//Get memory requirements
	vkGetBufferMemoryRequirements(*this->m_device->get_logical_device(), this->m_buffer, &this->m_memory_requirements);

	//Set up memory allocate info
	this->m_alloc_info.sType= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	this->m_alloc_info.pNext= nullptr;
	this->m_alloc_info.allocationSize= this->m_memory_requirements.size;
	this->m_alloc_info.memoryTypeIndex= this->m_device->find_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, this->m_memory_requirements.memoryTypeBits);

	this->m_device_memory= {};

	//Allocate memory
	result = vkAllocateMemory(*this->m_device->get_logical_device(), &this->m_alloc_info, nullptr, &this->m_device_memory);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not allocate device memory: " << result << std::endl;
#endif
		throw std::exception("Could not allocate device memory");
	}


	//Bind the memory
	result = vkBindBufferMemory(*this->m_device->get_logical_device(), this->m_buffer, this->m_device_memory, 0);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not bind device memory: " << result << std::endl;
#endif
		throw std::exception("Could not bind device memory");
	}

}


void c_buffer::fill(void *data, size_t size) {
	void *gpu_memory;
	vkMapMemory(*this->m_device->get_logical_device(), this->m_device_memory, 0, VK_WHOLE_SIZE, 0, &gpu_memory);

	if (size > this->m_size) {
		memcpy(gpu_memory, data, this->m_size);
	}
	else {
		memcpy(gpu_memory, data, size);
	}

	vkUnmapMemory(*this->m_device->get_logical_device(), this->m_device_memory);
}

void c_buffer::set(void *data, size_t size, uint32_t offset) {
	void *gpu_memory;
	vkMapMemory(*this->m_device->get_logical_device(), this->m_device_memory, offset, VK_WHOLE_SIZE, 0, &gpu_memory);

	if (size + offset > this->m_size) {
		memcpy(gpu_memory, data, this->m_size);
	}
	else {
		memcpy(gpu_memory, data, size);
	}

	vkUnmapMemory(*this->m_device->get_logical_device(), this->m_device_memory);
}

void c_buffer::clear() {
	void *gpu_memory;
	vkMapMemory(*this->m_device->get_logical_device(), this->m_device_memory, 0, VK_WHOLE_SIZE, 0, &gpu_memory);

	memset(gpu_memory, 0x00, this->m_size);

	vkUnmapMemory(*this->m_device->get_logical_device(), this->m_device_memory);
}

VkBuffer *c_buffer::get_buffer() {
	return &this->m_buffer;
}