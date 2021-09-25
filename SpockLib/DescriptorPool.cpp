#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(Device* device, VkDescriptorPoolSize* pool_sizes, uint32_t pool_size_count) :
	device(device), poolSizes(pool_sizes), poolSizeCount(pool_size_count) {

	this->poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	this->poolCreateInfo.pNext = nullptr;
	this->poolCreateInfo.flags = 0;
	this->poolCreateInfo.maxSets = 1000;	///TODO: Make this some reasonable number or something
	this->poolCreateInfo.poolSizeCount = this->poolSizeCount;
	this->poolCreateInfo.pPoolSizes = this->poolSizes;

	VkResult result = vkCreateDescriptorPool(*this->device->GetLogicalDevice(), &this->poolCreateInfo, nullptr, &this->descriptorPool);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create a descriptor pool: " << result << std::endl;
#endif 
		throw std::exception("Could not create descriptor pool");
	}

}

DescriptorSet DescriptorPool::ProduceDescriptorSet(SetLayout set_layout) {
	return DescriptorSet(this->device, set_layout);
	//return nullptr;//temporary
}