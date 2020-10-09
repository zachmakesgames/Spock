#pragma once
#include "vulkan/vulkan.h"

#include "ShaderPack.h"

#include "View.h"
#include "Device.h"
#include "DescriptorSet.h"


class Pipeline {
	public:
		Pipeline(Device* device, View* view, DescriptorSet* descriptorSet, int DescriptorSetCount, ShaderPack* pack);
		VkPipelineLayout GetPipelineLayout();
		VkPipeline* GetPipelineVK();


	private:
		VkResult result;

		Device* ThisDevice = nullptr;
		View* ThisView = nullptr;
		DescriptorSet* ThisDescriptorSet = nullptr;
		int DescriptorSetCount = 0;
		ShaderPack* Pack;

		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo;
		VkPipelineLayout PipelineLayout;

		VkPipelineShaderStageCreateInfo* ShaderStages = nullptr;

		VkVertexInputBindingDescription* VertexBindings = nullptr;

		VkVertexInputAttributeDescription* VertexAttributes = nullptr;

		VkPipelineVertexInputStateCreateInfo InputStateCreateInfo;

		VkViewport ViewPort;

		VkRect2D Scissor;

		VkPipelineViewportStateCreateInfo ViewportStateCreateInfo;

		VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo;

		VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo;

		VkPipelineColorBlendAttachmentState ColorBlendAttachementState;

		VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo;

		VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo;

		VkStencilOpState StencilFront;
		VkStencilOpState StencilBack;

		VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo;

		VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo;

		VkGraphicsPipelineCreateInfo PipelineCreateInfo;

		VkPipeline* PipelineVK = nullptr;

};