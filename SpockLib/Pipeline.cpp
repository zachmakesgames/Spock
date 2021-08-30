#include "Pipeline.h"

Pipeline::Pipeline(Instance* instance, Device* device, View* view, VkDescriptorSetLayout* set_layouts, uint32_t layout_count,
	VkPipelineShaderStageCreateInfo* shader_stages, uint32_t shader_stage_count,
	VkVertexInputBindingDescription* vertex_input_bindings, uint32_t vertex_input_count,
	VkVertexInputAttributeDescription* vertex_input_attributes, uint32_t vertex_input_attribute_count,
	VkRect2D scissor_rect,
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, uint32_t tesselation_patch_control_points = 0) :

	instance(instance), device(device), view(view), shaderStages(shader_stages), shaderStageCount(shader_stage_count),
	vertexInputBindings(vertex_input_bindings), vertexInputBindingCount(vertex_input_count),
	vertexInputAttributes(vertex_input_attributes), vertexInputAttributeCount(vertex_input_count),
	scissorRect(scissor_rect)
{

	VkResult result;

	//Setup pipeline layout create info
	this->layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	this->layoutCreateInfo.pNext = nullptr;
	this->layoutCreateInfo.flags = 0;
	this->layoutCreateInfo.setLayoutCount = this->layoutCount;
	this->layoutCreateInfo.pSetLayouts = this->setLayouts;
	this->layoutCreateInfo.pushConstantRangeCount = 0;	///TODO: Add option for push constants
	this->layoutCreateInfo.pPushConstantRanges = nullptr;

	result = vkCreatePipelineLayout(*this->device->GetLogicalDevice(), &this->layoutCreateInfo, nullptr, &this->pipelineLayout);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create a pipeline layout: " << result << std::endl;
#endif
		throw std::exception("Could not create pipeline layout");
	}

	//Set up the input state create info
	this->vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	this->vertexInputStateCreateInfo.pNext = nullptr;
	this->vertexInputStateCreateInfo.flags = 0;
	this->vertexInputStateCreateInfo.vertexBindingDescriptionCount = this->vertexInputBindingCount;
	this->vertexInputStateCreateInfo.pVertexBindingDescriptions = this->vertexInputBindings;
	this->vertexInputStateCreateInfo.vertexAttributeDescriptionCount = this->vertexInputAttributeCount;
	this->vertexInputStateCreateInfo.pVertexAttributeDescriptions = this->vertexInputAttributes;

	//Set up viewport
	VkExtent2D surface_extent = this->view->GetExtent2D();

	this->viewport.x = 0;
	this->viewport.y = 0;
	this->viewport.width = surface_extent.width;
	this->viewport.height = surface_extent.height;
	this->viewport.minDepth = 0;
	this->viewport.maxDepth = 1.0;

	//Set up the viewport state create info
	this->viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->viewportStateCreateInfo.pNext = nullptr;
	this->viewportStateCreateInfo.flags = 0;
	this->viewportStateCreateInfo.viewportCount = 1;
	this->viewportStateCreateInfo.pViewports = &this->viewport;
	this->viewportStateCreateInfo.scissorCount = 1;
	this->viewportStateCreateInfo.pScissors = &this->scissorRect;

	//Set up rasterization state create info
	this->rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	this->rasterizationStateCreateInfo.pNext = nullptr;
	this->rasterizationStateCreateInfo.flags = 0;
	this->rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	this->rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	this->rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; ///TODO: allow this to be changed by user
	this->rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;	///TODO: allow this to be changed by user
	this->rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	this->rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	this->rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	this->rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	this->rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	this->rasterizationStateCreateInfo.lineWidth = 1.0f; ///TODO: allow user to change line width

	//Set up multisample state create info
	this->multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	this->multisampleStateCreateInfo.pNext = nullptr;
	this->multisampleStateCreateInfo.flags = 0;
	this->multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	this->multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	this->multisampleStateCreateInfo.minSampleShading = 0;
	this->multisampleStateCreateInfo.pSampleMask = nullptr;
	this->multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;


	//Set up color blend attachment state
	this->colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	this->colorBlendAttachmentState.blendEnable = VK_FALSE;
	this->colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	this->colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	this->colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	this->colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	this->colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	this->colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	//Set up color blend state create info
	this->colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	this->colorBlendStateCreateInfo.pNext = nullptr;
	this->colorBlendStateCreateInfo.flags = 0;
	this->colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	this->colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	this->colorBlendStateCreateInfo.attachmentCount = 1;
	this->colorBlendStateCreateInfo.pAttachments = &this->colorBlendAttachmentState;
	this->colorBlendStateCreateInfo.blendConstants[0] = 0;
	this->colorBlendStateCreateInfo.blendConstants[1] = 0;
	this->colorBlendStateCreateInfo.blendConstants[2] = 0;
	this->colorBlendStateCreateInfo.blendConstants[3] = 0;

	//Set up dynamic state create info
	this->dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	this->dynamicStateCreateInfo.pNext = nullptr;
	this->dynamicStateCreateInfo.flags = 0;
	this->dynamicStateCreateInfo.dynamicStateCount = 0;
	this->dynamicStateCreateInfo.pDynamicStates = nullptr;	///TODO: Figure out what the hell this does

	//Set up front stencil
	this->stencilFront.failOp = VK_STENCIL_OP_KEEP;
	this->stencilFront.passOp = VK_STENCIL_OP_KEEP;
	this->stencilFront.depthFailOp = VK_STENCIL_OP_KEEP;
	this->stencilFront.compareOp = VK_COMPARE_OP_NEVER;
	this->stencilFront.compareMask = ~0;
	this->stencilFront.writeMask = ~0;
	this->stencilFront.reference = 0;

	//Set up back stencil
	this->stencilBack.failOp = VK_STENCIL_OP_KEEP;
	this->stencilBack.passOp = VK_STENCIL_OP_KEEP;
	this->stencilBack.depthFailOp = VK_STENCIL_OP_KEEP;
	this->stencilBack.compareOp = VK_COMPARE_OP_NEVER;
	this->stencilBack.compareMask = ~0;
	this->stencilBack.writeMask = ~0;
	this->stencilBack.reference = 0;

	//Set up depth stencil create info
	this->depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	this->depthStencilStateCreateInfo.pNext = nullptr;
	this->depthStencilStateCreateInfo.flags = 0;
	this->depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	this->depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	this->depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	this->depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	this->depthStencilStateCreateInfo.front = this->stencilFront;
	this->depthStencilStateCreateInfo.back = this->stencilBack;
	this->depthStencilStateCreateInfo.minDepthBounds = 0.0;	///TODO: unifi this with the view rect3D
	this->depthStencilStateCreateInfo.maxDepthBounds = 1.0;
	this->depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

	//Set up input assembly create info
	this->inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	this->inputAssemblyStateCreateInfo.pNext = nullptr;
	this->inputAssemblyStateCreateInfo.flags = 0;
	this->inputAssemblyStateCreateInfo.topology = topology;
	this->inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;	///TODO: allow primitive restart for strips

	//FINALLY
	//Set up the pipeline create info
	this->graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	this->graphicsPipelineCreateInfo.pNext = nullptr;
	this->graphicsPipelineCreateInfo.flags = 0;
	this->graphicsPipelineCreateInfo.stageCount = this->shaderStageCount;
	this->graphicsPipelineCreateInfo.pStages = this->shaderStages;
	this->graphicsPipelineCreateInfo.pVertexInputState = &this->vertexInputStateCreateInfo;
	this->graphicsPipelineCreateInfo.pInputAssemblyState = &this->inputAssemblyStateCreateInfo;
	this->graphicsPipelineCreateInfo.pTessellationState = nullptr; ///TODO: enable tessellation!
	this->graphicsPipelineCreateInfo.pViewportState = &this->viewportStateCreateInfo;
	this->graphicsPipelineCreateInfo.pRasterizationState = &this->rasterizationStateCreateInfo;
	this->graphicsPipelineCreateInfo.pMultisampleState = &this->multisampleStateCreateInfo;
	this->graphicsPipelineCreateInfo.pDepthStencilState = &this->depthStencilStateCreateInfo;
	this->graphicsPipelineCreateInfo.pColorBlendState = &this->colorBlendStateCreateInfo;
	this->graphicsPipelineCreateInfo.pDynamicState = &this->dynamicStateCreateInfo;
	this->graphicsPipelineCreateInfo.layout = this->pipelineLayout;
	this->graphicsPipelineCreateInfo.renderPass = *this->view->GetRenderPass();
	this->graphicsPipelineCreateInfo.subpass = 0;	///TODO: enable sub passes
	this->graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	this->graphicsPipelineCreateInfo.basePipelineIndex = 0;


	///TODO: Explore the creation of multiple pipelines at once
	result = vkCreateGraphicsPipelines(*this->device->GetLogicalDevice(), VK_NULL_HANDLE, 1, &this->graphicsPipelineCreateInfo, nullptr, &this->pipeline);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating a graphics pipeline: " << result << std::endl;
#endif
		throw std::exception("Could not create graphics pipeline");
	}
}

VkPipeline* Pipeline::GetVkPipeline() {
	return &this->pipeline;
}

void Pipeline::BreakPipeline() {
	vkDestroyPipeline(*this->device->GetLogicalDevice(), this->pipeline, nullptr);
}

void Pipeline::RebuildPipeline() {
	//Need to redefine anything that references the view so we update the screen size
	//Set up viewport
	VkExtent2D surface_extent = this->view->GetExtent2D();

	this->viewport.width = surface_extent.width;
	this->viewport.height = surface_extent.height;

	this->graphicsPipelineCreateInfo.renderPass = *this->view->GetRenderPass();


	//Rebuild pipeline with new surface size
	VkResult result = vkCreateGraphicsPipelines(*this->device->GetLogicalDevice(), VK_NULL_HANDLE, 1, &this->graphicsPipelineCreateInfo, nullptr, &this->pipeline);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating a graphics pipeline: " << result << std::endl;
#endif
		throw std::exception("Could not create graphics pipeline");
	}
}