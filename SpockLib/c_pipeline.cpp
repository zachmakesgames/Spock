#include "c_pipeline.h"

c_pipeline::c_pipeline(c_instance *instance, c_device *device, c_view *view, VkDescriptorSetLayout *set_layouts, uint32_t layout_count,
	VkPipelineShaderStageCreateInfo *shader_stages, uint32_t shader_stage_count,
	VkVertexInputBindingDescription *vertex_input_bindings, uint32_t vertex_input_count,
	VkVertexInputAttributeDescription *vertex_input_attributes, uint32_t vertex_input_attribute_count,
	VkRect2D scissor_rect,
	VkPrimitiveTopology topology= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, uint32_t tesselation_patch_control_points= 0) :

	m_instance(instance), m_device(device), m_view(view), m_set_layouts(set_layouts), m_layout_count(layout_count), m_shader_stages(shader_stages), m_shader_stage_count(shader_stage_count),
	m_vertex_input_bindings(vertex_input_bindings), m_vertex_input_binding_count(vertex_input_count),
	m_vertex_input_attributes(vertex_input_attributes), m_vertex_input_attribute_count(vertex_input_count),
	m_scissor_rect(scissor_rect)
{

	VkResult result;

	//Setup pipeline layout create info
	this->m_layout_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	this->m_layout_create_info.pNext= nullptr;
	this->m_layout_create_info.flags= 0;
	this->m_layout_create_info.setLayoutCount= this->m_layout_count;
	this->m_layout_create_info.pSetLayouts= this->m_set_layouts;
	this->m_layout_create_info.pushConstantRangeCount= 0;	///TODO: Add option for push constants
	this->m_layout_create_info.pPushConstantRanges= nullptr;

	result = vkCreatePipelineLayout(*this->m_device->get_logical_device(), &this->m_layout_create_info, nullptr, &this->m_pipeline_layout);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not create a pipeline layout: " << result << std::endl;
#endif
		throw std::exception("Could not create pipeline layout");
	}

	//Set up the input state create info
	this->m_vertex_input_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	this->m_vertex_input_state_create_info.pNext= nullptr;
	this->m_vertex_input_state_create_info.flags= 0;
	this->m_vertex_input_state_create_info.vertexBindingDescriptionCount= this->m_vertex_input_binding_count;
	this->m_vertex_input_state_create_info.pVertexBindingDescriptions= this->m_vertex_input_bindings;
	this->m_vertex_input_state_create_info.vertexAttributeDescriptionCount= this->m_vertex_input_attribute_count;
	this->m_vertex_input_state_create_info.pVertexAttributeDescriptions= this->m_vertex_input_attributes;

	//Set up viewport
	VkExtent2D surface_extent= this->m_view->get_extent_2D();

	this->m_viewport.x= 0;
	this->m_viewport.y= 0;
	this->m_viewport.width= surface_extent.width;
	this->m_viewport.height= surface_extent.height;
	this->m_viewport.minDepth= 0;
	this->m_viewport.maxDepth= 1.0;

	//Set up the viewport state create info
	this->m_viewport_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	this->m_viewport_state_create_info.pNext= nullptr;
	this->m_viewport_state_create_info.flags= 0;
	this->m_viewport_state_create_info.viewportCount= 1;
	this->m_viewport_state_create_info.pViewports= &this->m_viewport;
	this->m_viewport_state_create_info.scissorCount= 1;
	this->m_viewport_state_create_info.pScissors= &this->m_scissor_rect;

	//Set up rasterization state create info
	this->m_rasterization_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	this->m_rasterization_state_create_info.pNext= nullptr;
	this->m_rasterization_state_create_info.flags= 0;
	this->m_rasterization_state_create_info.depthClampEnable= VK_FALSE;
	this->m_rasterization_state_create_info.rasterizerDiscardEnable= VK_FALSE;
	this->m_rasterization_state_create_info.polygonMode= VK_POLYGON_MODE_FILL; ///TODO: allow this to be changed by user
	this->m_rasterization_state_create_info.cullMode= VK_CULL_MODE_NONE;	///TODO: allow this to be changed by user
	this->m_rasterization_state_create_info.frontFace= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	this->m_rasterization_state_create_info.depthBiasEnable= VK_FALSE;
	this->m_rasterization_state_create_info.depthBiasConstantFactor= 0.0f;
	this->m_rasterization_state_create_info.depthBiasClamp= 0.0f;
	this->m_rasterization_state_create_info.depthBiasSlopeFactor= 0.0f;
	this->m_rasterization_state_create_info.lineWidth= 1.0f; ///TODO: allow user to change line width

	//Set up multisample state create info
	this->m_multisample_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	this->m_multisample_state_create_info.pNext= nullptr;
	this->m_multisample_state_create_info.flags= 0;
	this->m_multisample_state_create_info.rasterizationSamples= VK_SAMPLE_COUNT_1_BIT;
	this->m_multisample_state_create_info.sampleShadingEnable= VK_FALSE;
	this->m_multisample_state_create_info.minSampleShading= 0;
	this->m_multisample_state_create_info.pSampleMask= nullptr;
	this->m_multisample_state_create_info.alphaToCoverageEnable= VK_FALSE;


	//Set up color blend attachment state
	this->m_color_blend_attachment_state.colorWriteMask= VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	this->m_color_blend_attachment_state.blendEnable= VK_FALSE;
	this->m_color_blend_attachment_state.srcColorBlendFactor= VK_BLEND_FACTOR_SRC_COLOR;
	this->m_color_blend_attachment_state.dstColorBlendFactor= VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	this->m_color_blend_attachment_state.colorBlendOp= VK_BLEND_OP_ADD;
	this->m_color_blend_attachment_state.srcAlphaBlendFactor= VK_BLEND_FACTOR_ONE;
	this->m_color_blend_attachment_state.dstAlphaBlendFactor= VK_BLEND_FACTOR_ZERO;
	this->m_color_blend_attachment_state.alphaBlendOp= VK_BLEND_OP_ADD;

	//Set up color blend state create info
	this->m_color_blend_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	this->m_color_blend_state_create_info.pNext= nullptr;
	this->m_color_blend_state_create_info.flags= 0;
	this->m_color_blend_state_create_info.logicOpEnable= VK_FALSE;
	this->m_color_blend_state_create_info.logicOp= VK_LOGIC_OP_COPY;
	this->m_color_blend_state_create_info.attachmentCount= 1;
	this->m_color_blend_state_create_info.pAttachments= &this->m_color_blend_attachment_state;
	this->m_color_blend_state_create_info.blendConstants[0]= 0;
	this->m_color_blend_state_create_info.blendConstants[1]= 0;
	this->m_color_blend_state_create_info.blendConstants[2]= 0;
	this->m_color_blend_state_create_info.blendConstants[3]= 0;

	//Set up dynamic state create info
	this->m_dynamic_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	this->m_dynamic_state_create_info.pNext= nullptr;
	this->m_dynamic_state_create_info.flags= 0;
	this->m_dynamic_state_create_info.dynamicStateCount= 0;
	this->m_dynamic_state_create_info.pDynamicStates= nullptr;	///TODO: Figure out what the hell this does

	//Set up front stencil
	this->m_stencil_front.failOp= VK_STENCIL_OP_KEEP;
	this->m_stencil_front.passOp= VK_STENCIL_OP_KEEP;
	this->m_stencil_front.depthFailOp= VK_STENCIL_OP_KEEP;
	this->m_stencil_front.compareOp= VK_COMPARE_OP_NEVER;
	this->m_stencil_front.compareMask= ~0;
	this->m_stencil_front.writeMask= ~0;
	this->m_stencil_front.reference= 0;

	//Set up back stencil
	this->m_stencil_back.failOp= VK_STENCIL_OP_KEEP;
	this->m_stencil_back.passOp= VK_STENCIL_OP_KEEP;
	this->m_stencil_back.depthFailOp= VK_STENCIL_OP_KEEP;
	this->m_stencil_back.compareOp= VK_COMPARE_OP_NEVER;
	this->m_stencil_back.compareMask= ~0;
	this->m_stencil_back.writeMask= ~0;
	this->m_stencil_back.reference= 0;

	//Set up depth stencil create info
	this->m_depth_stencil_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	this->m_depth_stencil_state_create_info.pNext= nullptr;
	this->m_depth_stencil_state_create_info.flags= 0;
	this->m_depth_stencil_state_create_info.depthTestEnable= VK_TRUE;
	this->m_depth_stencil_state_create_info.depthWriteEnable= VK_TRUE;
	this->m_depth_stencil_state_create_info.depthCompareOp= VK_COMPARE_OP_LESS;
	this->m_depth_stencil_state_create_info.depthBoundsTestEnable= VK_FALSE;
	this->m_depth_stencil_state_create_info.front= this->m_stencil_front;
	this->m_depth_stencil_state_create_info.back= this->m_stencil_back;
	this->m_depth_stencil_state_create_info.minDepthBounds= 0.0;	///TODO: unifi this with the view rect3D
	this->m_depth_stencil_state_create_info.maxDepthBounds= 1.0;
	this->m_depth_stencil_state_create_info.stencilTestEnable= VK_FALSE;

	//Set up input assembly create info
	this->m_input_assembly_state_create_info.sType= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	this->m_input_assembly_state_create_info.pNext= nullptr;
	this->m_input_assembly_state_create_info.flags= 0;
	this->m_input_assembly_state_create_info.topology= topology;
	this->m_input_assembly_state_create_info.primitiveRestartEnable= VK_FALSE;	///TODO: allow primitive restart for strips

	//FINALLY
	//Set up the pipeline create info
	this->m_graphics_pipeline_create_info.sType= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	this->m_graphics_pipeline_create_info.pNext= nullptr;
	this->m_graphics_pipeline_create_info.flags= 0;
	this->m_graphics_pipeline_create_info.stageCount= this->m_shader_stage_count;
	this->m_graphics_pipeline_create_info.pStages= this->m_shader_stages;
	this->m_graphics_pipeline_create_info.pVertexInputState= &this->m_vertex_input_state_create_info;
	this->m_graphics_pipeline_create_info.pInputAssemblyState= &this->m_input_assembly_state_create_info;
	this->m_graphics_pipeline_create_info.pTessellationState= nullptr; ///TODO: enable tessellation!
	this->m_graphics_pipeline_create_info.pViewportState= &this->m_viewport_state_create_info;
	this->m_graphics_pipeline_create_info.pRasterizationState= &this->m_rasterization_state_create_info;
	this->m_graphics_pipeline_create_info.pMultisampleState= &this->m_multisample_state_create_info;
	this->m_graphics_pipeline_create_info.pDepthStencilState= &this->m_depth_stencil_state_create_info;
	this->m_graphics_pipeline_create_info.pColorBlendState= &this->m_color_blend_state_create_info;
	this->m_graphics_pipeline_create_info.pDynamicState= &this->m_dynamic_state_create_info;
	this->m_graphics_pipeline_create_info.layout= this->m_pipeline_layout;
	this->m_graphics_pipeline_create_info.renderPass= *this->m_view->get_render_pass();
	this->m_graphics_pipeline_create_info.subpass= 0;	///TODO: enable sub passes
	this->m_graphics_pipeline_create_info.basePipelineHandle= VK_NULL_HANDLE;
	this->m_graphics_pipeline_create_info.basePipelineIndex= 0;


	///TODO: Explore the creation of multiple pipelines at once
	result = vkCreateGraphicsPipelines(*this->m_device->get_logical_device(), VK_NULL_HANDLE, 1, &this->m_graphics_pipeline_create_info, nullptr, &this->m_pipeline);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating a graphics pipeline: " << result << std::endl;
#endif
		throw std::exception("Could not create graphics pipeline");
	}
}

VkPipeline* c_pipeline::get_vk_pipeline() {
	return &this->m_pipeline;
}

void c_pipeline::break_pipeline() {
	vkDestroyPipeline(*this->m_device->get_logical_device(), this->m_pipeline, nullptr);
}

void c_pipeline::rebuild_pipeline() {
	//Need to redefine anything that references the view so we update the screen size
	//Set up viewport
	VkExtent2D surface_extent= this->m_view->get_extent_2D();

	this->m_viewport.width= surface_extent.width;
	this->m_viewport.height= surface_extent.height;

	this->m_graphics_pipeline_create_info.renderPass= *this->m_view->get_render_pass();


	//Rebuild pipeline with new surface size
	VkResult result = vkCreateGraphicsPipelines(*this->m_device->get_logical_device(), VK_NULL_HANDLE, 1, &this->m_graphics_pipeline_create_info, nullptr, &this->m_pipeline);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating a graphics pipeline: " << result << std::endl;
#endif
		throw std::exception("Could not create graphics pipeline");
	}
}