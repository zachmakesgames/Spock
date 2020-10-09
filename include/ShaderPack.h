#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <vulkan/vulkan.h>

#include <cstdint>
#include <string>
#include <sstream>
#include <regex>

#include <iostream>


#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat2x2.hpp"
#include "mat3x3.hpp"
#include "mat4x4.hpp"



#define VERSION 0x00000001

//make this something funny or something stupid you idiot
#define MAGIC 0xFF00FF00

#define MAX_POOL_SIZE 128 * 6

enum class LayoutStage {
	LAYOUT_VERTEX = 0b10000000,
	LAYOUT_TESSELLATION_CONTROL = 0b01000000,
	LAYOUT_TESSELLATION_EVALUATION = 0b00100000,
	LAYOUT_GEOMETRY = 0b00010000,
	LAYOUT_FRAGMENT = 0b00001000,
	LAYOUT_COMPUTE = 0b00000100
};

enum class GLSLTypeDir {
	D_IN = 0x00,
	D_OUT = 0x01,
	D_UNIFORM = 0x02
};


enum class GLSLType {
	BOOL = 0x00,
	INT = 0x01,
	UINT = 0x02,
	FLOAT = 0x03,
	DOUBLE = 0x04,
	BVEC2 = 0x05,
	BVEC3 = 0x06,
	BVEC4 = 0x07,
	IVEC2 = 0x08,
	IVEC3 = 0x09,
	IVEC4 = 0x0A,
	UVEC2 = 0x0B,
	UVEC3 = 0x0C,
	UVEC4 = 0x0D,
	VEC2 = 0x0E,
	VEC3 = 0x0F,
	VEC4 = 0x10,
	DVEC2 = 0x11,
	DVEC3 = 0x12,
	DVEC4 = 0x13,
	MAT2 = 0x14,
	MAT3 = 0x15,
	MAT4 = 0x16,
	SAMPLER1D = 0x17,
	SAMPLER2D = 0x18,
	SAMPLER3D = 0x19,
	SAMPLERCUBE = 0x1A,
	SAMPLER1DSHADOW = 0x1B,
	SAMPLER2DSHADOW = 0x1C,
	STRUCT = 0x1D,
	UNDEFINED = 0xFF
};




//A class for holding information about layout data from a GLSL shader
//This is really long and all inline and I am so sorry
class Layout
{
	
public:

	//TODO: Add == operator overload

	//Need this so ShaderPack can create an empty layout and populate members directly through friendship
	//DO NOT USE OUTSIDE OF ShaderPack CLASS
	inline Layout() {}

	//Made stages an optional argument, stages can be added later with AddStage();
	inline Layout(std::string RawIn, std::vector<LayoutStage> stages = std::vector<LayoutStage>()) {


		this->_stagesV = stages;	//this may not be the best way to do this
		this->_subLayouts.clear();	//clear out the sublayouts just in case

		//get rid of page feeds and line breaks
		std::regex oneliner("\r?\n|\r");
		std::string Raw = std::regex_replace(RawIn, oneliner, "");


		//To save time we expect that the raw input has already been validated 
		size_t openParen = Raw.find("(");	//find the open and closing parenthesis : eg layout  ->(std140, set = 0, binding = 0)<-
		size_t closeParen = Raw.find(")");

		//Use the found values above to get the strings in between the parenthesis
		std::string inParen = Raw.substr(openParen + 1, (closeParen - openParen) - 1);	//+1 and -1 so we dont get the () characters

		std::istringstream parenStream(inParen);
		std::string decoration;
		while (std::getline(parenStream, decoration, ',')) {
			//get each decoration from the layout
			size_t eq = decoration.find('=');


			if (eq != std::string::npos) {//theres an equal sign so we need to tokenize the decorator

				std::regex removeWhiteSpace("\\s+");
				std::string token = decoration.substr(0, eq);
				std::string value = decoration.substr(eq + 1, decoration.size());

				//Looked for a nicer solution to removing white space, but this seems to be
				//the most elegant and simple solution. Plus regex is fast
				token = std::regex_replace(token, removeWhiteSpace, "");
				value = std::regex_replace(value, removeWhiteSpace, "");


				//We can use a stringstream to convert to an integer! Fuckin A!
				std::stringstream valueStream = std::stringstream(value);


				//process based on token
				if (token == "location") {
					valueStream >> this->_location;
				}
				if (token == "component") {
					valueStream >> this->_component;
				}
				if (token == "set") {
					valueStream >> this->_set;
				}
				if (token == "binding") {
					valueStream >> this->_binding;
				}
			}
			else {	//no equals sign so maybe a packing?
				//do we actually care about how its packed right now? I dont think so
				///TODO: Impliment a tracker for packing type
			}
		}


		//find the brackets, if there are any
		size_t openBracket = Raw.find("{");
		size_t closeBracket = Raw.find("}");

		if (openBracket == std::string::npos) {
			//no opening bracket, just a single variable
			//So we can strip out the parenthesis and the ending ; and we should be left with three values only
			// in/out/uniform type name


			size_t tailEnd = Raw.find(';');	//This way we dont get the ending ;
			std::string tail = Raw.substr(closeParen + 1, tailEnd - closeParen - 1);	//remember that the second argument to substr is a length, not an offset!


			//We're going to sanitize the tail so we remove beginning and ending whitespace
			std::regex trimStart("^\\s+");		//Remember: Regular expressions need the character \ which needs to be escaped! so whenever you want a \ use a \\ instead 
			std::regex trimEnd("\\s+$");
			std::regex reduceSpaces("\\s+");

			tail = std::regex_replace(tail, trimStart, "");
			tail = std::regex_replace(tail, trimEnd, "");
			tail = std::regex_replace(tail, reduceSpaces, " ");	//replaces any cases of multiple spaces     with one space

			size_t spc1 = tail.find(" ");
			std::string dir = tail.substr(0, spc1);	//this should give us the direction in/out/uniform
													//remember that the second argument to substr is a length, not an offset!


			if (dir == "in") {
				this->_dir = GLSLTypeDir::D_IN;
			}
			if (dir == "out") {
				this->_dir = GLSLTypeDir::D_OUT;
			}
			if (dir == "uniform") {
				this->_dir = GLSLTypeDir::D_UNIFORM;
			}

			size_t spc2 = tail.find(" ", spc1 + 1);//find the next instance of a space by using the first space found


			std::string varType = tail.substr(spc1 + 1, spc2 - spc1 - 1); //remember that the second argument to substr is a length, not an offset!
			this->_type = StringToType(varType);


			std::string varName = tail.substr(spc2 + 1, tail.size() - spc2 + 1); //remember that the second argument to substr is a length, not an offset!
			this->_name = varName;
		}
		else {
			//An opening bracket so this is automatically a struct
			this->_type = GLSLType::STRUCT;

			//now find out if this is an in/out/uniform

			//Since we expect Raw to be all on one line, we can get the tail by getting the segment closeParen and openBracket
			size_t tailEnd = openBracket;	//This way we dont get the ending ;
			std::string tail = Raw.substr(closeParen + 1, tailEnd - closeParen - 1);	//remember that the second argument to substr is a length, not an offset!


			//We're going to sanitize the tail so we remove beginning and ending whitespace
			std::regex trimStart("^\\s+");		//Remember: Regular expressions need the character \ which needs to be escaped! so whenever you want a \ use a \\ instead 
			std::regex trimEnd("\\s+$");
			std::regex reduceSpaces("\\s+");

			tail = std::regex_replace(tail, trimStart, "");
			tail = std::regex_replace(tail, trimEnd, "");
			tail = std::regex_replace(tail, reduceSpaces, " ");	//replaces any cases of multiple spaces     with one space

			size_t spc1 = tail.find(" ");
			std::string dir = tail.substr(0, spc1);	//this should give us the direction in/out/uniform
													//remember that the second argument to substr is a length, not an offset!


			if (dir == "in") {
				this->_dir = GLSLTypeDir::D_IN;
			}
			if (dir == "out") {
				this->_dir = GLSLTypeDir::D_OUT;
			}
			if (dir == "uniform") {
				this->_dir = GLSLTypeDir::D_UNIFORM;
			}

			std::string varName = tail.substr(spc1 + 1, tail.size() - spc1 + 1); //remember that the second argument to substr is a length, not an offset!
			this->_name = varName;

			//get the number of variables in the struct

			//std::string innerVariables = Raw.substr(openBracket, closeBracket - openBracket - 1);	//remember the second argument is a length not an offset!
			std::string innerVariables = Raw.substr(openBracket, closeBracket - openBracket);
			size_t startLine = 0;
			size_t endLine = innerVariables.find(";");



			int innerVariableCount = 0;
			while (endLine != std::string::npos) {
				std::string innerVarRaw = "";

				++innerVariableCount;
				size_t endLineNew = innerVariables.find(";", endLine + 1);//find the next one
				innerVarRaw = innerVariables.substr(startLine + 1, endLine - startLine - 1);

				//clean up the string
				innerVarRaw = std::regex_replace(innerVarRaw, trimStart, "");
				innerVarRaw = std::regex_replace(innerVarRaw, trimEnd, "");
				innerVarRaw = std::regex_replace(innerVarRaw, reduceSpaces, " ");

				//then get the variable type and the variable name
				size_t findSpace = innerVarRaw.find(" ");
				if (findSpace != std::string::npos) {
					std::string varType = innerVarRaw.substr(0, findSpace);
					std::string varName = innerVarRaw.substr(findSpace + 1, innerVarRaw.length() - findSpace);

					Layout l(StringToType(varType), varName);
					//add the layout to this layout somehow
					this->_subLayouts.push_back(l);
					this->_subLayoutCount += 1;
				}

				startLine = endLine;
				endLine = endLineNew;

			}


		}
	}



	inline Layout(int32_t location, GLSLType t) {
		this->_location = location;
		this->_type = t;
	}

	/*inline Layout(int32_t location, int32_t component, GLSLType t) {
		this->_location = location;
		this->_component = component;
		this->_type = t;
	}*/

	inline Layout(int32_t set, int32_t binding, GLSLType t) {
		this->_set = set;
		this->_binding = binding;
		this->_type = t;
	}

	inline Layout(int32_t set, int32_t binding, int32_t component, GLSLType t) {
		this->_set = set;
		this->_binding = binding;
		this->_component = component;
		this->_type = t;
	}

	inline Layout(int32_t location, GLSLType t, std::string name) {
		this->_location = location;
		this->_type = t;
		this->_name = name;
	}

	//only good for struct members
	inline Layout(GLSLType t, std::string name) {
		this->_type = t;
		this->_name = name;
	}

	/*inline Layout(int32_t location, int32_t component, GLSLType t, std::string name) {
		this->_location = location;
		this->_component = component;
		this->_type = t;
		this->_name = name;
	}*/

	inline Layout(int32_t set, int32_t binding, GLSLType t, std::string name) {
		this->_set = set;
		this->_binding = binding;
		this->_type = t;
		this->_name = name;
	}

	inline Layout(int32_t set, int32_t binding, int32_t component, GLSLType t, std::string name) {
		this->_set = set;
		this->_binding = binding;
		this->_component = component;
		this->_type = t;
		this->_name = name;
	}
	inline Layout(int32_t location, int32_t set, int32_t binding, int32_t component, GLSLType t, GLSLTypeDir td, std::string name) {
		this->_set = set;
		this->_binding = binding;
		this->_component = component;
		if (t == GLSLType::UNDEFINED) {
			std::cout << "Woah\n";
		}
		this->_type = t;
		this->_name = name;
		this->_location = location;
		this->_dir = td;
	}

	inline int32_t Location() {
		return this->_location;
	}

	inline int32_t Set() {
		return this->_set;
	}

	inline int32_t Binding() {
		return this->_binding;
	}

	inline int32_t Component() {
		return this->_component;
	}

	inline GLSLTypeDir Direction() {
		return this->_dir;
	}

	inline std::string DirectionString() {
		switch (this->_dir) {
		case GLSLTypeDir::D_IN: return "in";
			break;
		case GLSLTypeDir::D_OUT: return "out";
			break;
		case GLSLTypeDir::D_UNIFORM: return "uniform";
			break;
		}
	}

	inline GLSLType Type() {
		return this->_type;
	}

	inline size_t TypeSize(GLSLType t) {
		switch (this->_type) {
		case GLSLType::BOOL: return sizeof(bool);
			break;
		case GLSLType::INT: return sizeof(int);
			break;
		case GLSLType::UINT: return sizeof(int);
			break;
		case GLSLType::FLOAT: return sizeof(float);
			break;
		case GLSLType::DOUBLE: return sizeof(double);
			break;
		case GLSLType::BVEC2: return sizeof(glm::bvec2);
			break;
		case GLSLType::BVEC3: return sizeof(glm::bvec3);
			break;
		case GLSLType::BVEC4: return sizeof(glm::bvec4);
			break;
		case GLSLType::IVEC2: return sizeof(glm::ivec2);
			break;
		case GLSLType::IVEC3: return sizeof(glm::ivec3);
			break;
		case GLSLType::IVEC4: return sizeof(glm::ivec4);
			break;
		case GLSLType::UVEC2: return sizeof(glm::uvec2);
			break;
		case GLSLType::UVEC3: return sizeof(glm::uvec3);
			break;
		case GLSLType::UVEC4: return sizeof(glm::uvec4);
			break;
		case GLSLType::VEC2: return sizeof(glm::vec2);
			break;
		case GLSLType::VEC3: return sizeof(glm::vec3);
			break;
		case GLSLType::VEC4: return sizeof(glm::vec4);
			break;
		case GLSLType::DVEC2: return sizeof(glm::dvec2);
			break;
		case GLSLType::DVEC3: return sizeof(glm::dvec3);
			break;
		case GLSLType::DVEC4: return sizeof(glm::dvec4);
			break;
		case GLSLType::MAT2: return sizeof(glm::mat2) * sizeof(float);
			break;
		case GLSLType::MAT3: return sizeof(glm::mat3) * sizeof(float);
			break;
		case GLSLType::MAT4: return sizeof(glm::mat4) * sizeof(float);
			break;
		default: return 0;
		}
	}

	inline std::string TypeString() {
		switch (this->_type) {
		case GLSLType::BOOL: return "bool";
			break;
		case GLSLType::INT: return "int";
			break;
		case GLSLType::UINT: return "uint";
			break;
		case GLSLType::FLOAT: return "float";
			break;
		case GLSLType::DOUBLE: return "double";
			break;
		case GLSLType::BVEC2: return "bvec2";
			break;
		case GLSLType::BVEC3: return "bvec3";
			break;
		case GLSLType::BVEC4: return "bvec4";
			break;
		case GLSLType::IVEC2: return "ivec2";
			break;
		case GLSLType::IVEC3: return "ivec3";
			break;
		case GLSLType::IVEC4: return "ivec4";
			break;
		case GLSLType::UVEC2: return "uvec2";
			break;
		case GLSLType::UVEC3: return "uvec3";
			break;
		case GLSLType::UVEC4: return "uvec4";
			break;
		case GLSLType::VEC2: return "vec2";
			break;
		case GLSLType::VEC3: return "vec3";
			break;
		case GLSLType::VEC4: return "vec4";
			break;
		case GLSLType::DVEC2: return "dvec2";
			break;
		case GLSLType::DVEC3: return "dvec3";
			break;
		case GLSLType::DVEC4: return "dvec4";
			break;
		case GLSLType::MAT2: return "mat2";
			break;
		case GLSLType::MAT3: return "mat3";
			break;
		case GLSLType::MAT4: return "mat4";
			break;
		case GLSLType::SAMPLER1D: return "sampler2D";
			break;
		case GLSLType::SAMPLER2D: return "sampler3D";
			break;
		case GLSLType::SAMPLER3D: return "sampler4D";
			break;
		case GLSLType::SAMPLERCUBE: return "samplercube";
			break;
		case GLSLType::SAMPLER1DSHADOW: return "sampler1Dshadow";
			break;
		case GLSLType::SAMPLER2DSHADOW: return "sampler2Dshadow";
			break;
		case GLSLType::STRUCT: return "struct";
			break;
		case GLSLType::UNDEFINED: return "undefined";
			break;
		default: return "NOT IMPLIMENTED";	//this is a great way to remember to add new types here, heheheheh
			break;
		}
	}

	inline std::string Name() {
		return this->_name;
	}


	inline GLSLType StringToType(std::string s) {
		if (s == "bool") return GLSLType::BOOL;
		if (s == "int") return GLSLType::INT;
		if (s == "uint") return GLSLType::UINT;
		if (s == "float") return GLSLType::FLOAT;
		if (s == "double") return GLSLType::DOUBLE;
		if (s == "bvec2") return GLSLType::BVEC2;
		if (s == "bvec3") return GLSLType::BVEC3;
		if (s == "bvec4") return GLSLType::BVEC4;
		if (s == "ivec2") return GLSLType::IVEC2;
		if (s == "ivec3") return GLSLType::IVEC3;
		if (s == "ivec4") return GLSLType::IVEC4;
		if (s == "uvec2") return GLSLType::UVEC2;
		if (s == "uvec3") return GLSLType::UVEC3;
		if (s == "uvec4") return GLSLType::UVEC4;
		if (s == "vec2") return GLSLType::VEC2;
		if (s == "vec3") return GLSLType::VEC3;
		if (s == "vec4") return GLSLType::VEC4;
		if (s == "dvec2") return GLSLType::DVEC2;
		if (s == "dvec3") return GLSLType::DVEC3;
		if (s == "dvec4") return GLSLType::DVEC4;
		if (s == "mat2") return GLSLType::MAT2;
		if (s == "mat3") return GLSLType::MAT3;
		if (s == "mat4") return GLSLType::MAT4;
		if (s == "sampler1D") return GLSLType::SAMPLER1D;
		if (s == "sampler2D") return GLSLType::SAMPLER2D;
		if (s == "sampler3D") return GLSLType::SAMPLER3D;
		if (s == "samplercube") return GLSLType::SAMPLERCUBE;
		if (s == "sampler1dshadow") return GLSLType::SAMPLER1DSHADOW;
		if (s == "sampler2dshadow") return GLSLType::SAMPLER2DSHADOW;
		if (s == "struct") return GLSLType::STRUCT;
		return GLSLType::UNDEFINED;
	}

	//DEPRICATED
	__declspec(deprecated) inline std::vector<LayoutStage> GetStagesV() {
		return this->_stagesV;
	}

	inline uint8_t GetStages() {
		return this->_stages;
	}

	inline std::string GetStagesString() {
		std::string stages;

		if (this->_stages & 0b10000000) {
			//vertx
			if (stages == "") stages += " | ";
			stages += "LAYOUT_VERTEX";
		}


		if (this->_stages & 0b01000000) {
			//tesc
			if (stages == "") stages += " | ";
			stages += "LAYOUT_TESSELLATION_CONTROL";
		}

		if (this->_stages & 0b00100000) {
			//tese
			if (stages == "") stages += " | ";
			stages += "LAYOUT_TESSELLATION_EVALUATION";
		}

		if (this->_stages & 0b00010000) {
			//geom
			if (stages == "") stages += " | ";
			stages += "LAYOUT_GEOMETRY";
		}

		if (this->_stages & 0b00001000) {
			//fragment
			if (stages == "") stages += " | ";
			stages += "LAYOUT_FRAGMENT";
		}

		if (this->_stages & 0b00000100) {
			//compute
			if (stages == "") stages += " | ";
			stages += "LAYOUT_COMPUTE";
		}

		return stages;
	}

	//DEPRICATED
	__declspec(deprecated) inline void AddStageV(LayoutStage stage) {
		for (int i = 0; i < this->_stagesV.size(); ++i) {
			if (this->_stagesV[i] == stage) {
				return;
			}
		}
		this->_stagesV.push_back(stage);
	}

	inline void AddStage(LayoutStage stage) {
		switch (stage) {
		case LayoutStage::LAYOUT_VERTEX: {
			this->_stages |= 0b10000000;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				this->_subLayouts[i]._stages |= 0b10000000;
			}
		}
									   break;
		case LayoutStage::LAYOUT_TESSELLATION_CONTROL: {
			this->_stages |= 0b01000000;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				this->_subLayouts[i]._stages |= 0b01000000;
			}
		}
													 break;
		case LayoutStage::LAYOUT_TESSELLATION_EVALUATION: {
			this->_stages |= 0b00100000;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				this->_subLayouts[i]._stages |= 0b00100000;
			}
		}
														break;
		case LayoutStage::LAYOUT_GEOMETRY: {
			this->_stages |= 0b00010000;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				this->_subLayouts[i]._stages |= 0b00010000;
			}
		}
										 break;
		case LayoutStage::LAYOUT_FRAGMENT: {
			this->_stages |= 0b00001000;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				this->_subLayouts[i]._stages |= 0b00001000;
			}
		}
										 break;
		case LayoutStage::LAYOUT_COMPUTE: {
			this->_stages |= 0b00000100;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				this->_subLayouts[i]._stages |= 0b00000100;
			}
		}
										break;
		}
	}

	inline void AddStages(uint8_t stageBits) {
		this->_stages = stageBits;
	}

	inline uint32_t SubLayoutCount() {
		return this->_subLayoutCount;
	}

	inline std::vector<Layout> GetSubLayouts() {
		return this->_subLayouts;
	}

	inline void AddSubLayout(Layout l) {
		this->_subLayouts.push_back(l);
		this->_subLayoutCount += 1;
	}

	inline size_t Size() {
		if (this->_type == GLSLType::STRUCT) {
			size_t totalSize = 0;
			for (int i = 0; i < this->_subLayoutCount; ++i) {
				totalSize += this->_subLayouts[i].Size();
			}
			return totalSize;
		}
		else {
			return TypeSize(this->_type);
		}
	}

private:
	int32_t _location = -1;
	int32_t _set = -1;
	int32_t _binding = -1;
	int32_t _component = -1;
	GLSLType _type = GLSLType::UNDEFINED;
	GLSLTypeDir _dir = GLSLTypeDir::D_UNIFORM;	//default to uniform
	std::string _name;

	std::vector<LayoutStage> _stagesV;

	uint8_t _stages = 0b00000000;

	uint32_t _subLayoutCount = 0;
	std::vector<Layout> _subLayouts;
};


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

