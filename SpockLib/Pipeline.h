#pragma once
#include "Device.h"
#include "View.h"
#include "DescriptorSet.h"
#include "Buffer.h"


class Pipeline {
	public:
		Pipeline(Instance* instance, Device* device, View* view, VkDescriptorSetLayout* set_layouts, uint32_t layout_count, 
			VkPipelineShaderStageCreateInfo* shader_stages, uint32_t shader_stage_count,
			VkVertexInputBindingDescription* vertex_input_bindings, uint32_t vertex_input_count,
			VkVertexInputAttributeDescription* vertex_input_attributes, uint32_t vertex_input_attribute_count,
			VkRect2D scissor_rect,
			VkPrimitiveTopology topology, uint32_t tesselation_patch_control_points);

		VkPipeline* GetVkPipeline();
		void BreakPipeline();
		void RebuildPipeline();


	private:
		Instance* instance;
		Device* device;
		View* view;

		VkPipelineLayoutCreateInfo layoutCreateInfo;
		VkPipelineLayout pipelineLayout;

		VkDescriptorSetLayout* setLayouts;
		uint32_t layoutCount;

		VkPipelineShaderStageCreateInfo* shaderStages;
		uint32_t shaderStageCount;

		VkVertexInputBindingDescription* vertexInputBindings;
		uint32_t vertexInputBindingCount;

		VkVertexInputAttributeDescription* vertexInputAttributes;
		uint32_t vertexInputAttributeCount;

		VkRect2D scissorRect;

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;

		VkViewport viewport;

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
		
		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

		VkStencilOpState stencilFront;
		VkStencilOpState stencilBack;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

		VkPipeline pipeline;
};