#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <regex>

#include <iostream>


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
class Layout
{

public:

	//TODO: Add == operator overload

	//Need this so ShaderPack can create an empty layout and populate members directly through friendship
	//DO NOT USE OUTSIDE OF ShaderPack CLASS
	inline Layout() {}

	//Made stages an optional argument, stages can be added later with AddStage();
	Layout(std::string RawIn, std::vector<LayoutStage> stages = std::vector<LayoutStage>());



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

	size_t TypeSize(GLSLType t);

	inline std::string TypeString();

	inline std::string Name() {
		return this->_name;
	}


	GLSLType StringToType(std::string s);

	//DEPRICATED
	__declspec(deprecated) inline std::vector<LayoutStage> GetStagesV() {
		return this->_stagesV;
	}

	inline uint8_t GetStages() {
		return this->_stages;
	}

	std::string GetStagesString();

	//DEPRICATED
	__declspec(deprecated) inline void AddStageV(LayoutStage stage) {
		for (int i = 0; i < this->_stagesV.size(); ++i) {
			if (this->_stagesV[i] == stage) {
				return;
			}
		}
		this->_stagesV.push_back(stage);
	}

	void AddStage(LayoutStage stage);

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
