#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "vulkan/vulkan.h"

#include "Layout.h"

#define VERSION 0x00000001

#define MAGIC 0xFF00FF00

#define MAX_POOL_SIZE 128 * 6



class ShaderPack
{
public:
	ShaderPack();	//Default constructor for building a new ShaderPack
	~ShaderPack();	//Destructor

	void DeSerialize(std::string packfilePath);	//deserialize a shaderpack from a file
	void DeSerialize(unsigned char* raw, unsigned int rawSize);	//deserialize from a raw data set
	void Serialize(std::string packfilePath);	//Serialize a shaderpack into a file

	//void Serialize1(std::string packfilePath);	//Serialize a shaderpack into a file
	//void DeSerialize1(unsigned char* raw, unsigned int rawSize);	//Serialize a shaderpack into a file

	//Adds a stage to the shader pack, returns true if success, false if the stage is already defined
	bool AddStage(unsigned char* rawfile, unsigned int rawfileSize, std::vector<Layout> stageLayouts, LayoutStage stage);

	//Check if a stage is defined
	bool IsStageDefined(LayoutStage l);

	std::vector<Layout> GetLayouts();
	std::vector<Layout> GetUniformLayouts();

	VkDescriptorPoolCreateInfo GetDescriptorPoolCreateInfo();

	VkDescriptorSetLayoutCreateInfo* GetDescriptorSetLayoutCreateInfo(int& array_size);

	VkShaderModuleCreateInfo GetShaderModuleCreateInfo(LayoutStage l);

	//static method to parse a glsl 
	static std::vector<Layout> ParseGLSL(std::string filePath);

	//some utility functions for parsing data
	static uint8_t _GetInt8(unsigned char* data, unsigned int offset);
	static uint32_t _GetInt32(unsigned char* data, unsigned int offset);
	static uint64_t _GetInt64(unsigned char* data, unsigned int offset);



private:
	//Private variables
	std::vector<Layout> Layouts;
	std::vector<Layout> uniformLayouts;
	std::vector<LayoutStage> DefinedStages;

	bool vertStageDefined = false;
	bool tescStageDefined = false;
	bool teseStageDefined = false;
	bool geomStageDefined = false;
	bool fragStageDefined = false;
	bool compStageDefined = false;

	unsigned char* vertStage = nullptr;
	unsigned char* tescStage = nullptr;
	unsigned char* teseStage = nullptr;
	unsigned char* geomStage = nullptr;
	unsigned char* fragStage = nullptr;
	unsigned char* compStage = nullptr;

	uint64_t vertStageSize = 0;
	uint64_t tescStageSize = 0;
	uint64_t teseStageSize = 0;
	uint64_t geomStageSize = 0;
	uint64_t fragStageSize = 0;
	uint64_t compStageSize = 0;

	//private functions
	void _DeSerialize(unsigned char* raw, int rawSize);





	void PopulateVertStage(unsigned char* rawfile, unsigned int rawfileSize);
	void PopulateTescStage(unsigned char* rawfile, unsigned int rawfileSize);
	void PopulateTeseStage(unsigned char* rawfile, unsigned int rawfileSize);
	void PopulateGeomStage(unsigned char* rawfile, unsigned int rawfileSize);
	void PopulateFragStage(unsigned char* rawfile, unsigned int rawfileSize);
	void PopulateCompStage(unsigned char* rawfile, unsigned int rawfileSize);


	int _NextStage(uint8_t currentStage, uint8_t stageFlags);

	void _AddStage(LayoutStage l);

	void _WriteData(void* data, size_t length, std::fstream& stream);

	VkDescriptorType _GLSLTypeToDescriptorType(GLSLType t);
	VkShaderStageFlagBits _UInt8ToShaderStageFlags(uint8_t flags);
	

	//pointers to internal structures that NEED to be freed at some point
	VkDescriptorPoolSize* poolSize = nullptr;	//Used in GetDescriptorPoolCreateInfo()
	VkDescriptorSetLayoutBinding* layoutBindings = nullptr; //Used in GetDescriptorSetLayoutCreateInfo()

};

