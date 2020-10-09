#include "../inc/DescriptorSet.h"

DescriptorSet::DescriptorSet(Device* d, VkDescriptorPool* pool, ShaderPack* pack) {
	
	//Initial class setup stuff
	this->ThisDevice = d;
	this->ThisPool = pool;
	this->ThisPack = pack;
	this->LayoutCount = 0;
	


	//Get the layout create info

	int layoutCount = 0;
	this->SetLayoutCreateInfo = this->ThisPack->GetDescriptorSetLayoutCreateInfo(layoutCount);

	this->LayoutCount = layoutCount;

	this->SetLayouts = new VkDescriptorSetLayout[layoutCount];
	for (int i = 0; i < layoutCount; ++i) {
		this->result = vkCreateDescriptorSetLayout(this->ThisDevice->GetLogicalDevice(), &(this->SetLayoutCreateInfo[i]), nullptr, &(this->SetLayouts[i]));
		///TODO: Do some debug here
	}

	//Allocate the layouts
	this->SetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	this->SetAllocateInfo.pNext = nullptr;
	this->SetAllocateInfo.descriptorPool = *(this->ThisPool);
	this->SetAllocateInfo.descriptorSetCount = layoutCount;
	this->SetAllocateInfo.pSetLayouts = this->SetLayouts;

	this->DescriptorSetVK = new VkDescriptorSet[layoutCount];
	this->result = vkAllocateDescriptorSets(this->ThisDevice->GetLogicalDevice(), &(this->SetAllocateInfo), this->DescriptorSetVK);

	//Get the actual layout data from the pack
	std::vector<Layout> layoutData = this->ThisPack->GetUniformLayouts();


	this->BufferInfo = new VkDescriptorBufferInfo[layoutCount];
	for (int i = 0; i < layoutCount; ++i) {
		Layout l = layoutData[i];
		

			//Really need to do some extra checking in case this is an image sampler
			Buffer* uniformBuffer = new Buffer(this->ThisDevice, l.Size(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			this->UniformBuffers.Insert(l.Name(), uniformBuffer);

			this->BufferInfo[i].buffer = uniformBuffer->VkBuffer();
			this->BufferInfo[i].offset = 0;
			this->BufferInfo[i].range = l.Size();
		
	}

	this->WriteDescriptors = new VkWriteDescriptorSet[layoutCount];
	for (int i = 0; i < layoutCount; ++i) {
		this->WriteDescriptors[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		this->WriteDescriptors[i].pNext = nullptr;
		this->WriteDescriptors[i].dstSet = this->DescriptorSetVK[i];
		this->WriteDescriptors[i].dstBinding = layoutData[i].Binding();
		this->WriteDescriptors[i].dstArrayElement = 0;
		this->WriteDescriptors[i].descriptorCount = 1;
		this->WriteDescriptors[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;	//need to set this dynamically
		this->WriteDescriptors[i].pBufferInfo = &(this->BufferInfo[i]);
		this->WriteDescriptors[i].pImageInfo = nullptr;	//if this is a sampler then this needs to get set dynamically
		this->WriteDescriptors[i].pTexelBufferView = nullptr;
	}

	vkUpdateDescriptorSets(this->ThisDevice->GetLogicalDevice(), layoutCount, this->WriteDescriptors, 0, nullptr);


}

VkDescriptorSet* DescriptorSet::GetDescriptorSetVK() {
	return this->DescriptorSetVK;
}

VkDescriptorSetLayout* DescriptorSet::GetDescriptorSetLayout(){
	return this->SetLayouts;

}

VkResult DescriptorSet::FillUniform(std::string uniformName, void* data, int size) {
	VkResult r = VK_SUCCESS;
	if (this->UniformBuffers.HasKey(uniformName)) {
		Buffer* b = this->UniformBuffers.Get(uniformName);
		b->Fill(data, size);
	}
	else {
		r = VK_ERROR_MEMORY_MAP_FAILED;
#ifdef DEBUG
		std::cout << "Failed to find a buffer in the descriptor set!\n";
#endif
	}

	return r;
}


VkResult DescriptorSet::SetUniform(std::string uniformName, void* data, int size, int offset) {
	VkResult r = VK_SUCCESS;
	if (this->UniformBuffers.HasKey(uniformName)) {
		Buffer* b = this->UniformBuffers.Get(uniformName);
		b->Set(data, size, offset);
	}
	else {
		r = VK_ERROR_MEMORY_MAP_FAILED;
#ifdef DEBUG
		std::cout << "Failed to find a buffer in the descriptor set!\n";
#endif
	}


	return r;
}

uint64_t DescriptorSet::GetDescriptorSetCount() {
	return this->LayoutCount;
}