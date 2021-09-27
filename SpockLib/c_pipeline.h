#pragma once
#include "c_device.h"
#include "c_view.h"
#include "c_descriptor_set.h"
#include "c_buffer.h"


class c_pipeline {
	public:
		c_pipeline(c_instance *instance, c_device *device, c_view *view, VkDescriptorSetLayout *set_layouts, uint32_t layout_count,
			VkPipelineShaderStageCreateInfo *shader_stages, uint32_t shader_stage_count,
			VkVertexInputBindingDescription *vertex_input_bindings, uint32_t vertex_input_count,
			VkVertexInputAttributeDescription *vertex_input_attributes, uint32_t vertex_input_attribute_count,
			VkRect2D scissor_rect,
			VkPrimitiveTopology topology, uint32_t tesselation_patch_control_points);

		VkPipeline *get_vk_pipeline();
		void break_pipeline();
		void rebuild_pipeline();


	private:
		c_instance *m_instance;
		c_device *m_device;
		c_view *m_view;

		VkPipelineLayoutCreateInfo m_layout_create_info;
		VkPipelineLayout m_pipeline_layout;

		VkDescriptorSetLayout *m_set_layouts;
		uint32_t m_layout_count;

		VkPipelineShaderStageCreateInfo *m_shader_stages;
		uint32_t m_shader_stage_count;

		VkVertexInputBindingDescription *m_vertex_input_bindings;
		uint32_t m_vertex_input_binding_count;

		VkVertexInputAttributeDescription *m_vertex_input_attributes;
		uint32_t m_vertex_input_attribute_count;

		VkRect2D m_scissor_rect;

		VkPipelineVertexInputStateCreateInfo m_vertex_input_state_create_info;

		VkViewport m_viewport;

		VkPipelineViewportStateCreateInfo m_viewport_state_create_info;

		VkPipelineRasterizationStateCreateInfo m_rasterization_state_create_info;

		VkPipelineMultisampleStateCreateInfo m_multisample_state_create_info;

		VkPipelineColorBlendAttachmentState m_color_blend_attachment_state;
		
		VkPipelineColorBlendStateCreateInfo m_color_blend_state_create_info;

		VkPipelineDynamicStateCreateInfo m_dynamic_state_create_info;

		VkStencilOpState m_stencil_front;
		VkStencilOpState m_stencil_back;

		VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state_create_info;

		VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state_create_info;

		VkGraphicsPipelineCreateInfo m_graphics_pipeline_create_info;

		VkPipeline m_pipeline;
};