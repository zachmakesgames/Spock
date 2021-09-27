#include "c_descriptor_set.h"

c_descriptor_set::c_descriptor_set(c_device *device, VkDescriptorPool *descriptor_pool, c_set_layout set_layout) :
	m_device(device), m_pool(descriptor_pool), m_set_layout(set_layout) {

	
}