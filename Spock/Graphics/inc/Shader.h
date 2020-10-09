#pragma once
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include <string>

//This may have been an outdated file before shaderpacks were developed
/*
#include "Device.h"
#include "Descriptor.h"

enum class ShaderType {
	SHADER_VERTEX,
	SHADER_TESSELLATION_CONTROL,
	SHADER_TESSELLATION_EVALUATION,
	SHADER_GEOMETRY,
	SHADER_FRAGMENT,
	SHADER_COMPUTE
};

class Shader {
	public:
		Shader(Device* d, std::string SPIRVFileName, ShaderType sType);
		void AddDescriptor(Descriptor D);
		bool HasDescriptor(Descriptor D);
		ShaderType Type();


	private:
		VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
		VkShaderModule* ShaderModule;


		Descriptor* Descriptors = nullptr;
		uint32_t DescriptorCount = 0;
		ShaderType ThisType;
};
*/