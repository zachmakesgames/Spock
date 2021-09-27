#include "c_descriptor_pool.h"

c_descriptor_pool::c_descriptor_pool(c_device *device, VkDescriptorPoolSize *pool_sizes, uint32_t pool_size_count) :
	m_device(device), m_pool_sizes(pool_sizes), m_pool_size_count(pool_size_count) {

	this->m_pool_create_info.sType= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	this->m_pool_create_info.pNext= nullptr;
	this->m_pool_create_info.flags= 0;
	this->m_pool_create_info.maxSets= 1000;	///TODO: Make this some reasonable number or something
	this->m_pool_create_info.poolSizeCount= this->m_pool_size_count;
	this->m_pool_create_info.pPoolSizes= this->m_pool_sizes;

	VkResult result= vkCreateDescriptorPool(*this->m_device->get_logical_device(), &this->m_pool_create_info, nullptr, &this->m_descriptor_pool);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create a descriptor pool: " << result << std::endl;
#endif 
		throw std::exception("Could not create descriptor pool");
	}

}

c_descriptor_set c_descriptor_pool::produce_descriptor_set(c_set_layout set_layout) {
	return c_descriptor_set(this->m_device, &this->m_descriptor_pool, set_layout);
	//return nullptr;//temporary
}