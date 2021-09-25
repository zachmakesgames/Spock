#include "DescriptorSet.h"

DescriptorSet::DescriptorSet(Device* device, VkDescriptorPool* descriptor_pool, SetLayout set_layout) :
	device(device), pool(descriptor_pool), setLayout(set_layout) {

	
}