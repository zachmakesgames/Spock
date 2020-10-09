#include "../inc/Shader.h"
#define DEBUG

/*
Shader::Shader(Device* d, std::string SPIRVFileName, ShaderType sType) {

	VkResult result;

	this->ThisType = sType;

	FILE* fp;
	(void)fopen_s(&fp, SPIRVFileName.c_str(), "rb");
	if (fp == NULL)
	{
#ifdef DEBUG
		std::cout << "Vulkan encountered an error opening a shader file" << std::endl;
#endif
		throw -1;
	}
	
	uint32_t magic;
	fread(&magic, 4, 1, fp);
	if (magic != 0x07230203)
	{
#ifdef DEBUG
		std::cout << "Vulkan encountered an invalid shader file" << std::endl;
#endif
		throw -1;
	}

	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	unsigned char* code = new unsigned char[size];
	fread(code, size, 1, fp);
	fclose(fp);

	
	this->ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	this->ShaderModuleCreateInfo.pNext = nullptr;
	this->ShaderModuleCreateInfo.flags = 0;
	this->ShaderModuleCreateInfo.codeSize = size;
	this->ShaderModuleCreateInfo.pCode = (uint32_t*)code;

	result = vkCreateShaderModule(d->GetLogicalDevice(), &this->ShaderModuleCreateInfo, nullptr, this->ShaderModule);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating a shader module: " << result << std::endl;
#endif
		throw result;
	}

	delete[] code;

}

ShaderType Shader::Type() {
	return this->ThisType;
}
*/