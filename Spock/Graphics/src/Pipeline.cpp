#include "../inc/Pipeline.h"


Pipeline::Pipeline(Device* device, View* view, DescriptorSet* descriptorSet, int descriptorSetCount, ShaderPack* pack) {
	this->ThisDevice = device;
	this->ThisView = view;
	this->ThisDescriptorSet = descriptorSet;
	this->DescriptorSetCount = descriptorSetCount;
	this->Pack = pack;

	//Setup the pipeline layout create info
	this->PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	this->PipelineLayoutCreateInfo.pNext = 0;
	this->PipelineLayoutCreateInfo.flags = 0;
	this->PipelineLayoutCreateInfo.flags = 0;
	this->PipelineLayoutCreateInfo.setLayoutCount = this->ThisDescriptorSet->GetDescriptorSetCount();
	this->PipelineLayoutCreateInfo.pSetLayouts = this->ThisDescriptorSet->GetDescriptorSetLayout();// descriptorSetLayouts;
	this->PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	this->PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


	this->result = vkCreatePipelineLayout(this->ThisDevice->GetLogicalDevice(), &(this->PipelineLayoutCreateInfo), nullptr, &(this->PipelineLayout));
	if (this->result != VK_SUCCESS) {
		//do something!
	}

	bool vert, tesc, tese, geom, frag, compute;
	vert = tesc = tese = geom = frag = compute = false;

	VkShaderModuleCreateInfo vertInfo, tescInfo, teseInfo, geomInfo, fragInfo, computeInfo;
	VkShaderModule vertSM, tescSM, teseSM, geomSM, fragSM, computeSM;
	vertSM = tescSM = teseSM = geomSM = fragSM = computeSM = {};
	int definedStages = 0;

	if (this->Pack->IsStageDefined(LayoutStage::LAYOUT_VERTEX)) {
		vert = true;
		vertInfo = this->Pack->GetShaderModuleCreateInfo(LayoutStage::LAYOUT_VERTEX);
		this->result = vkCreateShaderModule(this->ThisDevice->GetLogicalDevice(), &vertInfo, nullptr, &vertSM);
		++definedStages;
	}
	if (this->Pack->IsStageDefined(LayoutStage::LAYOUT_TESSELLATION_CONTROL)) {
		tesc = true;
		tescInfo = this->Pack->GetShaderModuleCreateInfo(LayoutStage::LAYOUT_TESSELLATION_CONTROL);
		this->result = vkCreateShaderModule(this->ThisDevice->GetLogicalDevice(), &tescInfo, nullptr, &tescSM);
		++definedStages;
	}
	if (this->Pack->IsStageDefined(LayoutStage::LAYOUT_TESSELLATION_EVALUATION)) {
		tese = true;
		teseInfo = this->Pack->GetShaderModuleCreateInfo(LayoutStage::LAYOUT_TESSELLATION_EVALUATION);
		this->result = vkCreateShaderModule(this->ThisDevice->GetLogicalDevice(), &teseInfo, nullptr, &teseSM);
		++definedStages;
	}
	if (this->Pack->IsStageDefined(LayoutStage::LAYOUT_GEOMETRY)) {
		geom = true;
		geomInfo = this->Pack->GetShaderModuleCreateInfo(LayoutStage::LAYOUT_GEOMETRY);
		this->result = vkCreateShaderModule(this->ThisDevice->GetLogicalDevice(), &geomInfo, nullptr, &geomSM);
		++definedStages;
	}
	if (this->Pack->IsStageDefined(LayoutStage::LAYOUT_FRAGMENT)) {
		frag = true;
		fragInfo = this->Pack->GetShaderModuleCreateInfo(LayoutStage::LAYOUT_FRAGMENT);
		this->result = vkCreateShaderModule(this->ThisDevice->GetLogicalDevice(), &fragInfo, nullptr, &fragSM);
		++definedStages;
	}
	if (this->Pack->IsStageDefined(LayoutStage::LAYOUT_COMPUTE)) {
		compute = true;
		computeInfo = this->Pack->GetShaderModuleCreateInfo(LayoutStage::LAYOUT_COMPUTE);
		this->result = vkCreateShaderModule(this->ThisDevice->GetLogicalDevice(), &computeInfo, nullptr, &computeSM);
		++definedStages;
	}


	this->ShaderStages = new VkPipelineShaderStageCreateInfo[definedStages];
	int idx = 0;
	if (vert) {
		this->ShaderStages[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->ShaderStages[idx].pNext = nullptr;
		this->ShaderStages[idx].flags = 0;
		this->ShaderStages[idx].stage = VK_SHADER_STAGE_VERTEX_BIT;
		this->ShaderStages[idx].module = vertSM;
		this->ShaderStages[idx].pName = "main";
		this->ShaderStages[idx].pSpecializationInfo = nullptr;
		++idx;
	}
	if (tesc) {
		this->ShaderStages[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->ShaderStages[idx].pNext = nullptr;
		this->ShaderStages[idx].flags = 0;
		this->ShaderStages[idx].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		this->ShaderStages[idx].module = tescSM;
		this->ShaderStages[idx].pName = "main";
		this->ShaderStages[idx].pSpecializationInfo = nullptr;
		++idx;
	}
	if (tese) {
		this->ShaderStages[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->ShaderStages[idx].pNext = nullptr;
		this->ShaderStages[idx].flags = 0;
		this->ShaderStages[idx].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		this->ShaderStages[idx].module = teseSM;
		this->ShaderStages[idx].pName = "main";
		this->ShaderStages[idx].pSpecializationInfo = nullptr;
		++idx;
	}
	if (geom) {
		this->ShaderStages[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->ShaderStages[idx].pNext = nullptr;
		this->ShaderStages[idx].flags = 0;
		this->ShaderStages[idx].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		this->ShaderStages[idx].module = geomSM;
		this->ShaderStages[idx].pName = "main";
		this->ShaderStages[idx].pSpecializationInfo = nullptr;
		++idx;
	}
	if (frag) {
		this->ShaderStages[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->ShaderStages[idx].pNext = nullptr;
		this->ShaderStages[idx].flags = 0;
		this->ShaderStages[idx].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		this->ShaderStages[idx].module = fragSM;
		this->ShaderStages[idx].pName = "main";
		this->ShaderStages[idx].pSpecializationInfo = nullptr;
		++idx;
	}
	if (compute) {
		this->ShaderStages[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->ShaderStages[idx].pNext = nullptr;
		this->ShaderStages[idx].flags = 0;
		this->ShaderStages[idx].stage = VK_SHADER_STAGE_COMPUTE_BIT;
		this->ShaderStages[idx].module = computeSM;
		this->ShaderStages[idx].pName = "main";
		this->ShaderStages[idx].pSpecializationInfo = nullptr;
		++idx;
	}


	this->VertexBindings = new VkVertexInputBindingDescription;
	this->VertexBindings->binding = 0;
	this->VertexBindings->stride = sizeof(float) * 8;
	this->VertexBindings->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	this->VertexAttributes = new VkVertexInputAttributeDescription[3];
	this->VertexAttributes[0].location = 0;
	this->VertexAttributes[0].binding = 0;
	this->VertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	this->VertexAttributes[0].offset = 0;

	this->VertexAttributes[1].location = 1;
	this->VertexAttributes[1].binding = 0;
	this->VertexAttributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	this->VertexAttributes[1].offset = sizeof(float) * 3;

	this->VertexAttributes[2].location = 2;
	this->VertexAttributes[2].binding = 0;
	this->VertexAttributes[2].format = VK_FORMAT_R32G32_SFLOAT;
	this->VertexAttributes[2].offset = sizeof(float) * 6;


	this->InputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	this->InputStateCreateInfo.pNext = nullptr;
	this->InputStateCreateInfo.flags = 0;
	this->InputStateCreateInfo.vertexBindingDescriptionCount = 1;
	this->InputStateCreateInfo.pVertexBindingDescriptions = this->VertexBindings;
	this->InputStateCreateInfo.vertexAttributeDescriptionCount = 3;
	this->InputStateCreateInfo.pVertexAttributeDescriptions = this->VertexAttributes;

	VkExtent2D ext = this->ThisDevice->GetSurfaceExtent();

	this->ViewPort.x = 0;
	this->ViewPort.y = 0;
	this->ViewPort.width = ext.width;
	this->ViewPort.height = ext.height;
	this->ViewPort.minDepth = 0;
	this->ViewPort.maxDepth = 1.0;

	this->Scissor.offset.x = 0;
	this->Scissor.offset.y = 0;
	this->Scissor.extent = ext;


	this->ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->ViewportStateCreateInfo.pNext = nullptr;
	this->ViewportStateCreateInfo.flags = 0;
	this->ViewportStateCreateInfo.viewportCount = 1;
	this->ViewportStateCreateInfo.pViewports = &(this->ViewPort);
	this->ViewportStateCreateInfo.scissorCount = 1;
	this->ViewportStateCreateInfo.pScissors = &(this->Scissor);


	this->RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	this->RasterizationStateCreateInfo.pNext = nullptr;
	this->RasterizationStateCreateInfo.flags = 0;
	this->RasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	this->RasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	this->RasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;// VK_POLYGON_MODE_FILL;
	this->RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;	//this is where we would set backface culling
	this->RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	this->RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	this->RasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	this->RasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	this->RasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	this->RasterizationStateCreateInfo.lineWidth = 1.0f;	//CHANGE LINE WIDTH HERE



	this->MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	this->MultisampleStateCreateInfo.pNext = 0;
	this->MultisampleStateCreateInfo.flags = 0;
	this->MultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	this->MultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	this->MultisampleStateCreateInfo.minSampleShading = 0;
	this->MultisampleStateCreateInfo.pSampleMask = nullptr;
	this->MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	this->MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;



	this->ColorBlendAttachementState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	this->ColorBlendAttachementState.blendEnable = VK_FALSE;
	this->ColorBlendAttachementState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	this->ColorBlendAttachementState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	this->ColorBlendAttachementState.colorBlendOp = VK_BLEND_OP_ADD;
	this->ColorBlendAttachementState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	this->ColorBlendAttachementState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	this->ColorBlendAttachementState.alphaBlendOp = VK_BLEND_OP_ADD;


	this->ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	this->ColorBlendStateCreateInfo.pNext = nullptr;
	this->ColorBlendStateCreateInfo.flags = 0;
	this->ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	this->ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	this->ColorBlendStateCreateInfo.attachmentCount = 1;
	this->ColorBlendStateCreateInfo.pAttachments = &(this->ColorBlendAttachementState);
	this->ColorBlendStateCreateInfo.blendConstants[0] = 0;
	this->ColorBlendStateCreateInfo.blendConstants[1] = 0;
	this->ColorBlendStateCreateInfo.blendConstants[2] = 0;
	this->ColorBlendStateCreateInfo.blendConstants[3] = 0;


	this->DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	this->DynamicStateCreateInfo.pNext = nullptr;
	this->DynamicStateCreateInfo.flags = 0;
	this->DynamicStateCreateInfo.dynamicStateCount = 0;
	this->DynamicStateCreateInfo.pDynamicStates = nullptr;


	//this->StencilFront
	this->StencilFront.failOp = VK_STENCIL_OP_KEEP;
	this->StencilFront.passOp = VK_STENCIL_OP_KEEP;
	this->StencilFront.depthFailOp = VK_STENCIL_OP_KEEP;
	this->StencilFront.compareOp = VK_COMPARE_OP_NEVER;
	this->StencilFront.compareMask = ~0;
	this->StencilFront.writeMask = ~0;
	this->StencilFront.reference = 0;

	//this->StencilBack
	this->StencilBack.failOp = VK_STENCIL_OP_KEEP;
	this->StencilBack.passOp = VK_STENCIL_OP_KEEP;
	this->StencilBack.depthFailOp = VK_STENCIL_OP_KEEP;
	this->StencilBack.compareOp = VK_COMPARE_OP_NEVER;
	this->StencilBack.compareMask = ~0;
	this->StencilBack.writeMask = ~0;
	this->StencilBack.reference = 0;

	//this->DepthStencilStateCreateInfo
	this->DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	this->DepthStencilStateCreateInfo.pNext = nullptr;
	this->DepthStencilStateCreateInfo.flags = 0;
	this->DepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	this->DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	this->DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	this->DepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	this->DepthStencilStateCreateInfo.front = this->StencilFront;
	this->DepthStencilStateCreateInfo.back = this->StencilBack;
	this->DepthStencilStateCreateInfo.minDepthBounds = 0.;
	this->DepthStencilStateCreateInfo.maxDepthBounds = 1.;
	this->DepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;


	//this->InputAssemblyStateCreateInfo
	this->InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	this->InputAssemblyStateCreateInfo.pNext = nullptr;
	this->InputAssemblyStateCreateInfo.flags = 0;
	this->InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	this->InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;


	//this->PipelineCreateInfo
	this->PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	this->PipelineCreateInfo.pNext = nullptr;
	this->PipelineCreateInfo.flags = 0;
	this->PipelineCreateInfo.stageCount = 2;
	this->PipelineCreateInfo.pStages = this->ShaderStages;
	this->PipelineCreateInfo.pVertexInputState = &(this->InputStateCreateInfo);
	this->PipelineCreateInfo.pInputAssemblyState = &(this->InputAssemblyStateCreateInfo);
	this->PipelineCreateInfo.pTessellationState = nullptr;
	this->PipelineCreateInfo.pViewportState = &(this->ViewportStateCreateInfo);
	this->PipelineCreateInfo.pRasterizationState = &(this->RasterizationStateCreateInfo);
	this->PipelineCreateInfo.pMultisampleState = &(this->MultisampleStateCreateInfo);
	this->PipelineCreateInfo.pDepthStencilState = &(this->DepthStencilStateCreateInfo);
	this->PipelineCreateInfo.pColorBlendState = &(this->ColorBlendStateCreateInfo);
	this->PipelineCreateInfo.pDynamicState = &(this->DynamicStateCreateInfo);
	this->PipelineCreateInfo.layout = this->PipelineLayout;
	this->PipelineCreateInfo.renderPass = *(this->ThisView->GetRenderPass());
	this->PipelineCreateInfo.subpass = 0;
	this->PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	this->PipelineCreateInfo.basePipelineIndex = 0;

	this->PipelineVK = new VkPipeline;
	this->result = vkCreateGraphicsPipelines(this->ThisDevice->GetLogicalDevice(), VK_NULL_HANDLE, 1, &(this->PipelineCreateInfo), nullptr, this->PipelineVK);
	if (this->result != VK_SUCCESS) {
		//DO SOMETHING!
	}
}


VkPipelineLayout Pipeline::GetPipelineLayout() {
	return this->PipelineLayout;
}
VkPipeline* Pipeline::GetPipelineVK() {
	return this->PipelineVK;
}