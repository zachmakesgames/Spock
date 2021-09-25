#include "SetLayout.h"

SetLayout::SetLayout(Device* device, std::vector<Descriptor> descriptors) : device(device), descriptors(descriptors) {

	this->setLayout = {};

	bindings = new VkDescriptorSetLayoutBinding[this->descriptors.size()];

	for (int i = 0; i < this->descriptors.size(); ++i) {
		bindings[i].binding = this->descriptors[i].binding;
		bindings[i].descriptorType = this->descriptors[i].descriptorType;
		bindings[i].descriptorCount = this->descriptors[i].count;
		bindings[i].stageFlags = this->descriptors[i].stageFlags;
		bindings[i].pImmutableSamplers = nullptr;	///TODO: Figure out what this does and what it needs to be
	}

	this->layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	this->layoutCreateInfo.pNext = nullptr;
	this->layoutCreateInfo.flags = 0;
	this->layoutCreateInfo.bindingCount = this->descriptors.size();
	this->layoutCreateInfo.pBindings = this->bindings;

	VkResult result = vkCreateDescriptorSetLayout(*this->device->GetLogicalDevice(), &this->layoutCreateInfo, nullptr, &this->setLayout);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create a descriptor set layout: " << result << std::endl;
#endif // DEBUG
		throw std::exception("Could not create descriptor set layout");
	}
}

SetLayout::~SetLayout() {
	delete this->bindings;
}

VkDescriptorSetLayout* SetLayout::GetLayout() {
	return &this->setLayout;
}

