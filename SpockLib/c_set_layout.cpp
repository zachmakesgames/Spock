#include "c_set_layout.h"

c_set_layout::c_set_layout(c_device *device, std::vector<c_descriptor> descriptors) : m_device(device), m_descriptors(descriptors) {

	this->m_set_layout= {};

	m_bindings= new VkDescriptorSetLayoutBinding[this->m_descriptors.size()];

	for (int i = 0; i < this->m_descriptors.size(); i++) {
		m_bindings[i].binding= this->m_descriptors[i].m_binding;
		m_bindings[i].descriptorType= this->m_descriptors[i].m_descriptor_type;
		m_bindings[i].descriptorCount= this->m_descriptors[i].m_count;
		m_bindings[i].stageFlags= this->m_descriptors[i].m_stage_flags;
		m_bindings[i].pImmutableSamplers= nullptr;	///TODO: Figure out what this does and what it needs to be
	}

	this->m_layout_create_info.sType= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	this->m_layout_create_info.pNext= nullptr;
	this->m_layout_create_info.flags= 0;
	this->m_layout_create_info.bindingCount= this->m_descriptors.size();
	this->m_layout_create_info.pBindings= this->m_bindings;

	VkResult result= vkCreateDescriptorSetLayout(*this->m_device->get_logical_device(), &this->m_layout_create_info, nullptr, &this->m_set_layout);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create a descriptor set layout: " << result << std::endl;
#endif // DEBUG
		throw std::exception("Could not create descriptor set layout");
	}
}

c_set_layout::~c_set_layout() {
	delete this->m_bindings;
}

VkDescriptorSetLayout *c_set_layout::get_layout() {
	return &this->m_set_layout;
}

