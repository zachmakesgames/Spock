#include "ShaderPack.h"

ShaderPack::ShaderPack() {

}

ShaderPack::~ShaderPack() {
	/* Memory is no longer managed by this class
	if (this->vertStage != nullptr) {
		delete[] this->vertStage;
	}
	if (this->tescStage != nullptr) {
		delete[] this->tescStage;
	}
	if (this->teseStage != nullptr) {
		delete[] this->teseStage;
	}
	if (this->geomStage != nullptr) {
		delete[] this->geomStage;
	}
	if (this->fragStage != nullptr) {
		delete[] this->fragStage;
	}
	if (this->compStage != nullptr) {
		delete[] this->compStage;
	}
	*/
}

std::vector<Layout> ShaderPack::GetLayouts() {
	return this->Layouts;
}

std::vector<Layout> ShaderPack::GetUniformLayouts(){
	return this->uniformLayouts;
}


//Deserialize from a given file path
void ShaderPack::DeSerialize(std::string packfilePath) {

	
	std::ifstream instream(packfilePath, std::ios::binary | std::ios::ate);
	if (!instream.is_open()) {
		std::cout << "Failed to open file for deserialization\n";
		return;
	}

	uint64_t size = instream.tellg();
	char* code = new char[size];

	instream.seekg(0, std::ios::beg);
	instream.read(code, size);

#ifdef DEBUG
	std::cout << "Opened packfile of size: "  << size << "\n";
#endif

	this->_DeSerialize((unsigned char*)code, size);
}


//Deserialize from a raw bytestream
void ShaderPack::DeSerialize(unsigned char* raw, unsigned int rawSize) {
	this->_DeSerialize(raw, rawSize);
}


//Initialize this object with the information found in the given shaderpack binary data
void ShaderPack::_DeSerialize(unsigned char* raw, int rawSize) {
	uint32_t magic = 0x00;
	uint32_t version = 0x00;
	uint8_t stages = 0x00;

	uint64_t stageoffsets[6];
	stageoffsets[0] = 0x00;
	stageoffsets[1] = 0x00;
	stageoffsets[2] = 0x00;
	stageoffsets[3] = 0x00;
	stageoffsets[4] = 0x00;
	stageoffsets[5] = 0x00;

	uint64_t layoutOffset = 0x00;
	uint32_t layoutCount = 0x00;



	magic = _GetInt32(raw, 0x00);
	version = _GetInt32(raw, 0x04);

	stages = _GetInt8(raw, 0x08);
	stageoffsets[0] = _GetInt64(raw, 0x09);
	stageoffsets[1] = _GetInt64(raw, 0x11);
	stageoffsets[2] = _GetInt64(raw, 0x19);
	stageoffsets[3] = _GetInt64(raw, 0x21);
	stageoffsets[4] = _GetInt64(raw, 0x29);
	stageoffsets[5] = _GetInt64(raw, 0x31);

	layoutOffset = _GetInt64(raw, 0x39);

	layoutCount = _GetInt32(raw, 0x41);



	if (stages & 0b10000000) {
		//vertx
		this->vertStageDefined = true;
		int nextStage = _NextStage(0b10000000, stages);
		if (nextStage == 0) {
			this->vertStageSize = layoutOffset - stageoffsets[0];
		}
		else {
			this->vertStageSize = stageoffsets[nextStage] - stageoffsets[0];
		}

		this->vertStage = new unsigned char[this->vertStageSize];
		memset(this->vertStage, 0, this->vertStageSize);
		memcpy(this->vertStage, raw + stageoffsets[0], this->vertStageSize);
	}


	if (stages & 0b01000000) {
		//tesc
		this->tescStageDefined = true;
		int nextStage = _NextStage(0b01000000, stages);
		if (nextStage == 0) {
			this->tescStageSize = layoutOffset - stageoffsets[1];
		}
		else {
			this->tescStageSize = stageoffsets[nextStage] - stageoffsets[1];
		}

		this->tescStage = new unsigned char[this->tescStageSize];
		memset(this->tescStage, 0, this->tescStageSize);
		memcpy(this->tescStage, raw + stageoffsets[1], this->tescStageSize);
	}

	if (stages & 0b00100000) {
		//tese
		this->teseStageDefined = true;
		int nextStage = _NextStage(0b00100000, stages);
		if (nextStage == 0) {
			this->teseStageSize = layoutOffset - stageoffsets[2];
		}
		else {
			this->teseStageSize = stageoffsets[nextStage] - stageoffsets[2];
		}

		this->teseStage = new unsigned char[this->teseStageSize];
		memset(this->teseStage, 0, this->teseStageSize);
		memcpy(this->teseStage, raw + stageoffsets[2], this->teseStageSize);
	}

	if (stages & 0b00010000) {
		//geom
		this->geomStageDefined = true;
		int nextStage = _NextStage(0b00010000, stages);
		if (nextStage == 0) {
			this->geomStageSize = layoutOffset - stageoffsets[3];
		}
		else {
			this->geomStageSize = stageoffsets[nextStage] - stageoffsets[3];
		}

		this->geomStage = new unsigned char[this->geomStageSize];
		memset(this->geomStage, 0, this->geomStageSize);
		memcpy(this->geomStage, raw + stageoffsets[3], this->geomStageSize);
	}

	if (stages & 0b00001000) {
		//fragment
		this->fragStageDefined = true;
		int nextStage = _NextStage(0b00001000, stages);
		if (nextStage == 0) {
			this->fragStageSize = layoutOffset - stageoffsets[4];

		}
		else {
			this->fragStageSize = stageoffsets[nextStage] - stageoffsets[4];
		}

		this->fragStage = new unsigned char[this->fragStageSize];
		memset(this->fragStage, 0, this->fragStageSize);
		memcpy(this->fragStage, raw + stageoffsets[4], this->fragStageSize);
	}

	if (stages & 0b00000100) {
		//compute
		this->compStageDefined = true;
		int nextStage = _NextStage(0b00000100, stages);
		if (nextStage == 0) {
			this->compStageSize = layoutOffset - stageoffsets[5];

		}
		else {
			this->compStageSize = stageoffsets[nextStage] - stageoffsets[5];
		}

		this->compStage = new unsigned char[this->compStageSize];
		memset(this->compStage, 0, this->compStageSize);
		memcpy(this->compStage, raw + stageoffsets[5], this->compStageSize);
	}


	this->Layouts.clear(); // just in case someone is doing stupid things like testing the same instance over and over again -_- (*cough zach cough*) 
	uint64_t lastLayoutOffset = layoutOffset;

	for (int i = 0; i < layoutCount; ++i) {
		//std::cout << "----Parsing layout at offset: " << std::dec << lastLayoutOffset << "\n";

		uint32_t nameOffset = 0x1B;	//was 0x17 before adding sub layout count

		uint32_t layoutSize = _GetInt32(raw, lastLayoutOffset);

		//std::cout << "------Layout has size: " << std::dec << layoutSize << "\n";

		int32_t layoutLocation = _GetInt32(raw, lastLayoutOffset + 4);
		int32_t layoutSet = _GetInt32(raw, lastLayoutOffset + 8);
		int32_t layoutBinding = _GetInt32(raw, lastLayoutOffset + 12);
		int32_t layoutComponent = _GetInt32(raw, lastLayoutOffset + 16);
		int32_t subLayoutCount = _GetInt32(raw, lastLayoutOffset + 20);


		uint8_t layoutType = _GetInt8(raw, lastLayoutOffset + 24);

	
		uint8_t layoutDir = _GetInt8(raw, lastLayoutOffset + 25);
		uint8_t layoutStages = _GetInt8(raw, lastLayoutOffset + 26);


		std::string name;
		if (layoutSize - nameOffset > 1) {
			int stringSize = layoutSize - nameOffset;


			for (int i = 0; i < stringSize; ++i) {
				char c = (char)_GetInt8(raw, lastLayoutOffset + nameOffset + i);
				name += c;
			}

		}
		//std::cout << "-----Layout name: " << name << "\n";

		

		Layout l(layoutLocation, layoutSet, layoutBinding, layoutComponent, GLSLType(layoutType), GLSLTypeDir(layoutDir), name);
		l.AddStages(layoutStages);

		lastLayoutOffset += layoutSize;
		//Get all the sub layouts
		//uint32_t subLayoutOffset = lastLayoutOffset + layoutSize;
		for (int i = 0; i < subLayoutCount; ++i) {
			uint32_t subnameOffset = 0x1B;	//was 0x17 before adding sub layout count

			uint32_t sublayoutSize = _GetInt32(raw, lastLayoutOffset);

			//std::cout << "------Layout has size: " << std::dec << layoutSize << "\n";

			int32_t sublayoutLocation = _GetInt32(raw, lastLayoutOffset + 4);
			int32_t sublayoutSet = _GetInt32(raw, lastLayoutOffset + 8);
			int32_t sublayoutBinding = _GetInt32(raw, lastLayoutOffset + 12);
			int32_t sublayoutComponent = _GetInt32(raw, lastLayoutOffset + 16);
			int32_t subsubLayoutCount = _GetInt32(raw, lastLayoutOffset + 20);


			uint8_t sublayoutType = _GetInt8(raw, lastLayoutOffset + 24);
			uint8_t sublayoutDir = _GetInt8(raw, lastLayoutOffset + 25);
			uint8_t sublayoutDStages = _GetInt8(raw, lastLayoutOffset + 26);


			std::string subname;
			if (sublayoutSize - subnameOffset > 1) {
				int substringSize = sublayoutSize - subnameOffset;


				for (int i = 0; i < substringSize; ++i) {
					char c = (char)_GetInt8(raw, lastLayoutOffset + subnameOffset + i);
					subname += c;
				}

			}
			Layout sl(sublayoutLocation, sublayoutSet, sublayoutBinding, sublayoutComponent, GLSLType(sublayoutType), GLSLTypeDir(sublayoutDir), subname);
			sl.AddStages(layoutStages);


			l.AddSubLayout(sl);

			//subLayoutOffset = subLayoutOffset + sublayoutSize;
			lastLayoutOffset += sublayoutSize;
		}

		


		this->Layouts.push_back(l);



		//lastLayoutOffset += (uint64_t)layoutSize + (uint64_t)subLayoutOffset;
		//lastLayoutOffset = (uint64_t)layoutSize + (uint64_t)subLayoutOffset;
	}

#ifdef DEBUG
	std::cout << "@@ READ @@\n";
	std::cout << "    magic: " << std::dec << magic << "\n";
	std::cout << "    version: " << std::dec << version << "\n";
	std::cout << "    stages: " << std::dec << stages << "\n";
	std::cout << "    vertexOffset: " << std::dec << stageoffsets[0] << "\n";
	std::cout << "    tesConOffset: " << std::dec << stageoffsets[1] << "\n";
	std::cout << "    tesEvalOffset: " << std::dec << stageoffsets[2] << "\n";
	std::cout << "    geomOffset: " << std::dec << stageoffsets[3] << "\n";
	std::cout << "    fragOffset: " << std::dec << stageoffsets[4] << "\n";
	std::cout << "    computeOffset: " << std::dec << stageoffsets[5] << "\n";
	std::cout << "    layoutOffset: " << std::dec << layoutOffset << "\n";
	std::cout << "    layout count: " << std::dec << layoutCount << "\n";

	std::cout << "    vert size: " << std::dec << this->vertStageSize << "\n";
	std::cout << "    frag size: " << std::dec << this->fragStageSize << "\n";
#endif
}


//Serialize the object into a shaderpack file
void ShaderPack::Serialize(std::string packfilePath) {

	//uint64_t baseoffset = 0x67;
	uint64_t baseoffset = 0x5F;


	uint32_t magic = MAGIC;
	uint32_t version = 1;
	uint8_t stages = 0x00;

	uint64_t stageoffsets[6];
	stageoffsets[0] = 0x00;
	stageoffsets[1] = 0x00;
	stageoffsets[2] = 0x00;
	stageoffsets[3] = 0x00;
	stageoffsets[4] = 0x00;
	stageoffsets[5] = 0x00;


	uint64_t layoutOffset = 0x00;
	uint32_t layoutCount = 0x00;


	uint64_t lastDefinedStage = baseoffset;

	if (this->vertStageDefined) {
		stages |= 0b10000000;
		stageoffsets[0] = lastDefinedStage;
		lastDefinedStage += this->vertStageSize;
	}

	if (this->tescStageDefined) {
		stages |= 0b01000000;
		stageoffsets[1] = lastDefinedStage;
		lastDefinedStage += this->tescStageSize;
	}

	if (this->teseStageDefined) {
		stages |= 0b00100000;
		stageoffsets[2] = lastDefinedStage;
		lastDefinedStage += this->teseStageSize;
	}

	if (this->geomStageDefined) {
		stages |= 0b00010000;
		stageoffsets[3] = lastDefinedStage;
		lastDefinedStage += this->geomStageSize;
	}

	if (this->fragStageDefined) {
		stages |= 0b00001000;
		stageoffsets[4] = lastDefinedStage;
		lastDefinedStage += this->fragStageSize;
	}

	if (this->compStageDefined) {
		stages |= 0b00000100;
		stageoffsets[5] = lastDefinedStage;
		lastDefinedStage += this->compStageSize;
	}



	layoutOffset = lastDefinedStage;
	layoutCount = this->Layouts.size();


	uint8_t nullpad[26];
	memset(nullpad, 0x00, 26);


	std::ofstream outstream;
	outstream.open(packfilePath, std::ios::binary | std::ios::trunc);

	if (!outstream.is_open()) {
		std::cout << "Could not open packfile: " << packfilePath << "\n";
		std::cout << errno << "\n";
		return;
	}

	outstream.write((const char*)&magic, 4);
	outstream.write((const char*)&version, 4);
	outstream.write((const char*)&stages, 1);
	outstream.write((const char*)stageoffsets, 8 * 6);
	outstream.write((const char*)&layoutOffset, 8);
	outstream.write((const char*)&layoutCount, 4);

	outstream.write((const char*)nullpad, 26);

	if (this->vertStageDefined) {
		outstream.write((const char*)this->vertStage, this->vertStageSize);
	}

	if (this->fragStageDefined) {
		outstream.write((const char*)this->fragStage, this->fragStageSize);
	}

	

	//write the layouts
	for (int i = 0; i < this->Layouts.size(); ++i) {
		Layout l = this->Layouts[i];
		uint32_t layoutSize = 27 + l.Name().length();
		uint32_t layoutLocation = l.Location();
		uint32_t layoutSet = l.Set();
		uint32_t layoutBinding = l.Binding();
		uint32_t layoutComponent = l.Component();
		uint32_t subLayoutCount = l.SubLayoutCount();
		uint8_t layoutType = (uint8_t)l.Type();
		uint8_t layoutDir = (uint8_t)l.Direction();
		

		uint8_t layoutStages = 0x00;

		layoutStages = l.GetStages();


		outstream.write(reinterpret_cast<char*>(&layoutSize), 4);
		outstream.write(reinterpret_cast<char*>(&layoutLocation), 4);
		outstream.write(reinterpret_cast<char*>(&layoutSet), 4);
		outstream.write(reinterpret_cast<char*>(&layoutBinding), 4);
		outstream.write(reinterpret_cast<char*>(&layoutComponent), 4);
		outstream.write(reinterpret_cast<char*>(&subLayoutCount), 4);

		outstream.write(reinterpret_cast<char*>(&layoutType), 1);
		outstream.write(reinterpret_cast<char*>(&layoutDir), 1);
		outstream.write(reinterpret_cast<char*>(&layoutStages), 1);

		uint8_t nullterm = 0;
		char namearr[255];// = l.Name().c_str();

		int nameLength = l.Name().length();

		//Get l.Name() once and use it multiple times instead of like 15 gazillion calls to Name()
		memset(namearr, 0, 255);
		memcpy(namearr, l.Name().c_str(), nameLength > 255 ? 255 : nameLength);
		outstream.write(reinterpret_cast<char*>(&namearr), nameLength);

		//Special note: These strings don't have null terminators because std::strings don't have them.
		//Since this is a tightly packed file we go by the length instead


		//Get any sub layouts and write them to file
		std::vector<Layout> subLayouts = l.GetSubLayouts();

		for (int i = 0; i < subLayoutCount; ++i) {
			Layout sl = subLayouts[i];
			uint32_t layoutSize = 27 + sl.Name().length();
			uint32_t layoutLocation = l.Location();
			uint32_t layoutSet = sl.Set();
			uint32_t layoutBinding = sl.Binding();
			uint32_t layoutComponent = sl.Component();
			uint32_t subLayoutCount = 0;	//A sub layout shouldnt have any of its own sub layouts so set to 0
			uint8_t layoutType = (uint8_t)sl.Type();
			uint8_t layoutDir = (uint8_t)sl.Direction();


			uint8_t layoutStages = 0x00;

			layoutStages = sl.GetStages();


			outstream.write(reinterpret_cast<char*>(&layoutSize), 4);
			outstream.write(reinterpret_cast<char*>(&layoutLocation), 4);
			outstream.write(reinterpret_cast<char*>(&layoutSet), 4);
			outstream.write(reinterpret_cast<char*>(&layoutBinding), 4);
			outstream.write(reinterpret_cast<char*>(&layoutComponent), 4);
			outstream.write(reinterpret_cast<char*>(&subLayoutCount), 4);

			outstream.write(reinterpret_cast<char*>(&layoutType), 1);
			outstream.write(reinterpret_cast<char*>(&layoutDir), 1);
			outstream.write(reinterpret_cast<char*>(&layoutStages), 1);

			uint8_t nullterm = 0;
			char namearr[255];// = l.Name().c_str();
			memset(namearr, 0, 255);

			int subnameLength = sl.Name().length();

			memcpy(namearr, sl.Name().c_str(), subnameLength > 255 ? 255 : subnameLength);
			outstream.write(reinterpret_cast<char*>(&namearr), subnameLength);
			
			//NO NULL TERMINATOR!!!
		}

	}


	outstream.flush();
	outstream.close();

#ifdef DEBUG
	std::cout << "## WROTE ##\n";
	std::cout << "    magic: " << std::dec << magic << "\n";
	std::cout << "    version: " << std::dec << version << "\n";
	std::cout << "    stages: " << std::dec << stages << "\n";
	std::cout << "    vertexOffset: " << std::dec << stageoffsets[0] << "\n";
	std::cout << "    tesConOffset: " << std::dec << stageoffsets[1] << "\n";
	std::cout << "    tesEvalOffset: " << std::dec << stageoffsets[2] << "\n";
	std::cout << "    geomOffset: " << std::dec << stageoffsets[3] << "\n";
	std::cout << "    fragOffset: " << std::dec << stageoffsets[4] << "\n";
	std::cout << "    computeOffset: " << std::dec << stageoffsets[5] << "\n";
	std::cout << "    layoutOffset: " << std::dec << layoutOffset << "\n";
	std::cout << "    layout count: " << std::dec << layoutCount << "\n";


	std::cout << "    vert size: " << std::dec << this->vertStageSize << "\n";
	std::cout << "    frag size: " << std::dec << this->fragStageSize << "\n";
#endif
}

//Adds a stage to the shader pack if the stage is not already defined
bool ShaderPack::AddStage(unsigned char* rawfile, unsigned int rawfileSize, std::vector<Layout> stageLayouts, LayoutStage stage) {

	//verify that the stage is not already defined, and if it is return false
	for (int i = 0; i < this->DefinedStages.size(); ++i) {
		if (this->DefinedStages[i] == stage) {
			return false;
		}
	}

	

	//Then add the layouts to our list of layouts
	for (int i = 0; i < stageLayouts.size(); ++i) {
		//make sure we dont add any duplicates
		for (int j = 0; j < this->Layouts.size(); ++j) {
			Layout ll = this->Layouts[j];
			Layout lr = stageLayouts[i];
			if (ll.Name() == lr.Name() &&
				ll.Location() == lr.Location() &&
				ll.Set() == lr.Set() &&
				ll.Binding() == lr.Binding() &&
				ll.Component() == lr.Component() &&
				ll.Type() == lr.Type() &&
				ll.Direction() == lr.Direction()) {
				//I need to write an == override for this....
				//Anyways, if this is true then we have a duplicate layout so just update the stages
				ll.AddStage(stage);	//ll is the one in our set already
				break;	//break out and move on to the next layout in the input list
			}
		}


		Layout l = stageLayouts[i];
		l.AddStage(stage);	//ensure that the layout has the stage info
		this->Layouts.push_back(l);
	}

	this->_AddStage(stage);

	//populate our internal memory based on what stage is being added
	switch (stage) {
	case LayoutStage::LAYOUT_VERTEX: this->PopulateVertStage(rawfile, rawfileSize);
		break;
	case LayoutStage::LAYOUT_TESSELLATION_CONTROL: this->PopulateTescStage(rawfile, rawfileSize);
		break;
	case LayoutStage::LAYOUT_TESSELLATION_EVALUATION: this->PopulateTeseStage(rawfile, rawfileSize);
		break;
	case LayoutStage::LAYOUT_GEOMETRY: this->PopulateGeomStage(rawfile, rawfileSize);
		break;
	case LayoutStage::LAYOUT_FRAGMENT: this->PopulateFragStage(rawfile, rawfileSize);
		break;
	case LayoutStage::LAYOUT_COMPUTE: this->PopulateCompStage(rawfile, rawfileSize);
		break;
	}

	return true;
}


bool ShaderPack::IsStageDefined(LayoutStage l) {
	switch (l) {
		case LayoutStage::LAYOUT_VERTEX: return this->vertStageDefined;
			break;
		case LayoutStage::LAYOUT_FRAGMENT: return this->fragStageDefined;
			break;
		case LayoutStage::LAYOUT_GEOMETRY: return this->geomStageDefined;
			break;
		case LayoutStage::LAYOUT_TESSELLATION_CONTROL: return this->tescStageDefined;
			break;
		case LayoutStage::LAYOUT_TESSELLATION_EVALUATION: return this->teseStageDefined;
			break;
		case LayoutStage::LAYOUT_COMPUTE: return this->compStageDefined;
			break;
		default: return false;
			break;
	}
}


//Get a VkDescriptorPoolCreateInfo that can be used to set up a Vulkan Pipeline
VkDescriptorPoolCreateInfo ShaderPack::GetDescriptorPoolCreateInfo() {
	///TODO: Samplers cause this to create an undefined value in the poolsize and vulkan pukes
	if (this->Layouts.size() == 0) {
		throw(new std::exception("Shaderpack has no layouts!\n"));
		return {};	//maybe want to throw an error here, but whatever
	}


	this->uniformLayouts.clear();

	//Find the uniforms
	for (int i = 0; i < this->Layouts.size(); ++i) {
		Layout l = this->Layouts[i];
		if (l.Direction() == GLSLTypeDir::D_UNIFORM) {
			this->uniformLayouts.push_back(l);
		}
	}

	//poolSize = new VkDescriptorPoolSize[uniformLayouts.size()];
	//VkDescriptorPoolSize poolSize[MAX_POOL_SIZE];	//instead of using dynamic memory here, use a fixed array to avoid memory leaks, 128*6 is the hard limit on resources
	//memset(poolSize, 0, sizeof(VkDescriptorPoolSize) * 128*6);
	this->poolSize = new VkDescriptorPoolSize[this->uniformLayouts.size()];
	memset(this->poolSize, 0, sizeof(VkDescriptorPoolSize) * this->uniformLayouts.size());

	for (int i = 0; i < this->uniformLayouts.size(); ++i) {
		poolSize[i].descriptorCount = 1;
		Layout uniformL = this->uniformLayouts[i];

		GLSLType t = uniformL.Type();

		poolSize[i].type = _GLSLTypeToDescriptorType(t);

		if (poolSize[i].type == 255) {
			std::cout << "Something went wrong!\n";
			
		}
		/*
		switch (uniformL.Type()) {
			case GLSLType::SAMPLER1D:
			case GLSLType::SAMPLER1DSHADOW:
			case GLSLType::SAMPLER2D:
			case GLSLType::SAMPLER2DSHADOW:
			case GLSLType::SAMPLER3D:
			case GLSLType::SAMPLERCUBE: poolSize[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;
			case GLSLType::BOOL:
			case GLSLType::INT:
			case GLSLType::UINT:
			case GLSLType::FLOAT:
			case GLSLType::DOUBLE:
			case GLSLType::BVEC2:
			case GLSLType::BVEC3:
			case GLSLType::BVEC4:
			case GLSLType::IVEC2:
			case GLSLType::IVEC3:
			case GLSLType::IVEC4:
			case GLSLType::UVEC2:
			case GLSLType::UVEC3:
			case GLSLType::UVEC4:
			case GLSLType::VEC2:
			case GLSLType::VEC3:
			case GLSLType::VEC4:
			case GLSLType::DVEC2:
			case GLSLType::DVEC3:
			case GLSLType::DVEC4:
			case GLSLType::MAT2:
			case GLSLType::MAT3:
			case GLSLType::MAT4:
			case GLSLType::STRUCT: poolSize[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
		}
		*/
	}


	VkDescriptorPoolCreateInfo poolCreateInfo;
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.flags = 0; //otherwise VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT?
	poolCreateInfo.maxSets = this->uniformLayouts.size();
	poolCreateInfo.poolSizeCount = this->uniformLayouts.size();	//poolSize may actually be bigger, but anything past uniformLayouts.size() will be empty
	poolCreateInfo.pPoolSizes = poolSize;

	return poolCreateInfo;

}



//Get an array of VkDescriptorSetLayoutCreateInfo for the layouts contained in this shader pack
//Returns a dynamic array and sets array_size to the size of the returned array
VkDescriptorSetLayoutCreateInfo* ShaderPack::GetDescriptorSetLayoutCreateInfo(int& array_size) {

	int uniformCount = this->uniformLayouts.size();

	this->layoutBindings = new VkDescriptorSetLayoutBinding[uniformCount];

	for (int i = 0; i < uniformCount; ++i) {
		this->layoutBindings[i].binding = this->uniformLayouts[i].Binding();
		this->layoutBindings[i].descriptorType = _GLSLTypeToDescriptorType(this->uniformLayouts[i].Type());
		this->layoutBindings[i].descriptorCount = 1;
		//this->layoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;// _UInt8ToShaderStageFlags(this->uniformLayouts[i].GetStages());
		this->layoutBindings[i].stageFlags = _UInt8ToShaderStageFlags(this->uniformLayouts[i].GetStages());
		this->layoutBindings[i].pImmutableSamplers = nullptr;
	}

	VkDescriptorSetLayoutCreateInfo* layoutCreateInfo = new VkDescriptorSetLayoutCreateInfo[uniformCount];
	memset(layoutCreateInfo, 0, sizeof(VkDescriptorSetLayoutCreateInfo) * uniformCount);

	for (int i = 0; i < uniformCount; ++i) {
		layoutCreateInfo[i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo[i].pNext = nullptr;
		layoutCreateInfo[i].flags = 0;
		layoutCreateInfo[i].bindingCount = 1;
		layoutCreateInfo[i].pBindings = &this->layoutBindings[i];
	}

	array_size = uniformCount;
	return layoutCreateInfo;
}


//Get an array of VkShaderModuleCreateInfo for the spv shaders defined in this pack
VkShaderModuleCreateInfo ShaderPack::GetShaderModuleCreateInfo(LayoutStage l) {
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0;

	uint64_t codeSize = 0;
	uint32_t* code = nullptr;

	switch (l) {
	case LayoutStage::LAYOUT_VERTEX: {codeSize = this->vertStageSize; code = (uint32_t*)this->vertStage; }
		break;
	case LayoutStage::LAYOUT_FRAGMENT: {codeSize = this->fragStageSize; code = (uint32_t*)this->fragStage; }
		break;
	case LayoutStage::LAYOUT_GEOMETRY: {codeSize = this->geomStageSize; code = (uint32_t*)this->geomStage; }
		break;
	case LayoutStage::LAYOUT_TESSELLATION_CONTROL: {codeSize = this->tescStageSize; code = (uint32_t*)this->tescStage; }
		break;
	case LayoutStage::LAYOUT_TESSELLATION_EVALUATION: {codeSize = this->teseStageSize; code = (uint32_t*)this->teseStage; }
		break;
	case LayoutStage::LAYOUT_COMPUTE: {codeSize = this->compStageSize; code = (uint32_t*)this->compStage; }
		break;
	default: return {};
		break;
	}

	shaderModuleCreateInfo.codeSize = codeSize;
	shaderModuleCreateInfo.pCode = code;
	return shaderModuleCreateInfo;
	
}


//A static member function to get the layouts from a GLSL file
std::vector<Layout> ShaderPack::ParseGLSL(std::string filePath) {
	std::ifstream inReader;
	inReader.open(filePath);

	if (!inReader.is_open()) {
		throw(std::runtime_error("Could not open file for packing\n"));
	}

	std::string line = "";
	bool inMultiLineComment = false;
	bool inMultiLineStatement = false;


	std::string layoutDeclaration = "";

	std::vector<Layout> layouts;
	layouts.clear();

	//read each line of the file
	while (std::getline(inReader, line)) {

		//if we're in a multi-line comment check for the end of the comment before continuing
		if (inMultiLineComment) {
			if (line.find("*/") != std::string::npos) {
				inMultiLineComment = false;
				continue;
			}
		}
		if (line.find("//") == 0) continue;	//if start of line is a comment then go to next line
		if (line.find("#") == 0) continue;	//if line is a directive then go to next line
		if (line.find("/*") != std::string::npos) {
			inMultiLineComment = true;
			continue;
		}



		if (inMultiLineStatement) {
			//std::cout << "#*IN MULTILINE STATEMENT*#\n";
			if (line.find("}") != std::string::npos) {
				inMultiLineStatement = false;
				layoutDeclaration += line;
				layouts.push_back(Layout(layoutDeclaration));
				layoutDeclaration = "";
			}
			else {
				layoutDeclaration += line;
			}
			//std::cout << line << "\n";


			continue;
		}
		if (line.find("layout(") != std::string::npos || line.find("layout (") != std::string::npos) {	//need to do a little more than just check for the word layout
			if (line.find("in") != std::string::npos || line.find("out") != std::string::npos || line.find("uniform") != std::string::npos) {
				if ((line.find("{") != std::string::npos && line.find("}") == std::string::npos) || line.find(';') == std::string::npos)
				{
					inMultiLineStatement = true;
					layoutDeclaration += line;
				}
				else {

					layouts.push_back(Layout(line));
				}


			}
			continue;
		}
		else {
			continue;
		}

	}

	return layouts;
}


//There is no validation done here, its up to you the programmer
//to make sure you dont go outside the bounds of the array
//....or dont, see if I care
uint8_t ShaderPack::_GetInt8(unsigned char* data, unsigned int offset) {
	//std::cout << "Attempting to get 1 byte at offset " << offset << "\n";
	uint8_t converted = (uint8_t)(data[offset]);

	return converted;
}

uint32_t ShaderPack::_GetInt32(unsigned char* data, unsigned int offset) {
	//std::cout << "Attempting to get 4 byte at offset " << offset << "\n";
	uint32_t converted = (uint32_t)(data[offset]);
	uint32_t b2 = data[offset + 1];
	uint32_t b3 = data[offset + 2];
	uint32_t b4 = data[offset + 3];
	converted |= (b2 << 8);
	converted |= (b3 << 16);
	converted |= (b4 << 24);

	return converted;
}

uint64_t ShaderPack::_GetInt64(unsigned char* data, unsigned int offset) {
	//std::cout << "Attempting to get 8 bytes at offset " << offset << "\n";
	uint64_t converted = (uint64_t)(data[offset]);
	uint64_t b2 = data[offset + 1];
	uint64_t b3 = data[offset + 2];
	uint64_t b4 = data[offset + 3];
	uint64_t b5 = data[offset + 4];
	uint64_t b6 = data[offset + 5];
	uint64_t b7 = data[offset + 6];
	uint64_t b8 = data[offset + 7];
	converted |= (b2 << 8);
	converted |= (b3 << 16);
	converted |= (b4 << 24);
	converted |= (b5 << 32);
	converted |= (b6 << 40);
	converted |= (b7 << 48);
	converted |= (b8 << 56);

	return converted;
}



//Instead of doing memory copies out the fucking ass, just take in the pointers and
//copy only the pointer, not the whole file.
void ShaderPack::PopulateVertStage(unsigned char* rawfile, unsigned int rawfileSize) {
	this->vertStage = rawfile; //new unsigned char[rawfileSize];
	this->vertStageSize = rawfileSize;
	/*
	for (int i = 0; i < rawfileSize; ++i) {
		this->vertStage[i] = rawfile[i];
	}*/
}
void ShaderPack::PopulateTescStage(unsigned char* rawfile, unsigned int rawfileSize) {
	this->tescStage = rawfile; //new unsigned char[rawfileSize];
	this->tescStageSize = rawfileSize;
	/*
	for(int i = 0; i < rawfileSize; ++i){
		this->tescStage[i] = rawfile[i];
	}*/
}
void ShaderPack::PopulateTeseStage(unsigned char* rawfile, unsigned int rawfileSize) {
	this->teseStage = rawfile; //new unsigned char[rawfileSize];
	this->teseStageSize = rawfileSize;
	/*
	for(int i = 0; i < rawfileSize; ++i){
		this->teseStage[i] = rawfile[i];
	}*/
}
void ShaderPack::PopulateGeomStage(unsigned char* rawfile, unsigned int rawfileSize) {
	this->geomStage = rawfile; //new unsigned char[rawfileSize];
	this->geomStageSize = rawfileSize;
	/*
	for(int i = 0; i < rawfileSize; ++i){
		this->geomStage[i] = rawfile[i];
	}*/
}
void ShaderPack::PopulateFragStage(unsigned char* rawfile, unsigned int rawfileSize) {
	this->fragStage = rawfile; //new unsigned char[rawfileSize];
	this->fragStageSize = rawfileSize;
	/*
	for(int i = 0; i < rawfileSize; ++i){
		this->fragStage[i] = rawfile[i];
	}*/
}
void ShaderPack::PopulateCompStage(unsigned char* rawfile, unsigned int rawfileSize) {
	this->compStage = rawfile; //new unsigned char[rawfileSize];
	this->compStageSize = rawfileSize;
	/*
	for(int i = 0; i < rawfileSize; ++i){
		this->compStage[i] = rawfile[i];
	}*/
}


//A utility to determine the next stage defined in the pack file
//returns the next stage in the chain. If there are no remaining stages
//then it returns the vertex stage because it could never really be the next
//stage. This is a nice trick to denote the end of the shaders and the start
//of the layout block
int ShaderPack::_NextStage(uint8_t currentStage, uint8_t stageFlags) {

	uint8_t finalMask = 0b00000001;	//should be equal to 1 but endianness i guess?

	//create a mask to mask off all stages before the current stage
	uint8_t previousStages = currentStage;
	int numShifts = 0;
	//to do this shift the current stage until its equal to one and 
	//record the number of times we had to shift it
	while (previousStages != finalMask) {
		previousStages = previousStages >> 1;
		++numShifts;
	}

	//then shift 0b11111111 << the number of shifts which will mask off 
	//all previous stages
	uint8_t previousMask = 0xFF << numShifts;

	//Then the remaining stages to consider can be retreived by anding the inverse of the mask
	uint8_t remainingStages = stageFlags & ~previousMask;

	//Then do a similar shift to find the first flagged stage and return it
	numShifts = 0;
	while (remainingStages > 1) {
		remainingStages = remainingStages >> 1;
		++numShifts;
	}

	uint8_t nextStage = finalMask << numShifts;

	switch (nextStage) {
	case 0b01000000: return 1;
		break;
	case 0b00100000: return 2;
		break;
	case 0b00010000: return 3;
		break;
	case 0b00001000: return 4;
		break;
	case 0b00000100: return 5;
		break;
	default: return 0;
		break;
	}
	//return finalMask << numShifts;


}


//A safe way to add a layout stage to our list
void ShaderPack::_AddStage(LayoutStage l) {
	//Vectors are ruining my life, undermining my relationship with my children, trashing my marriage,
	//wrecking my cars, and fucking my dog.
	/*for (int i = 0; i < this->DefinedStages.size(); ++i) {
		if (this->DefinedStages[i] == l) {
			return false;
		}
	}

	this->DefinedStages.push_back(l);
	*/
	switch (l) {
	case LayoutStage::LAYOUT_VERTEX:  this->vertStageDefined = true;
		break;
	case LayoutStage::LAYOUT_TESSELLATION_CONTROL: this->tescStageDefined = true;
		break;
	case LayoutStage::LAYOUT_TESSELLATION_EVALUATION: this->teseStageDefined = true;
		break;
	case LayoutStage::LAYOUT_GEOMETRY: this->geomStageDefined = true;
		break;
	case LayoutStage::LAYOUT_FRAGMENT: this->fragStageDefined = true;
		break;
	case LayoutStage::LAYOUT_COMPUTE: this->compStageDefined = true;
		break;
	}
}


void ShaderPack::_WriteData(void* data, size_t length, std::fstream& stream) {
	if (stream.is_open()) {
		stream.write((const char*)data, length);
		if (stream.tellp() < length) {
			std::cout << "didnt write all the data? expected " << length << " wrote " << stream.tellp() << "\n";
		}
	}
	else {
		std::cout << "Stream not open\n";
	}
}


VkDescriptorType ShaderPack::_GLSLTypeToDescriptorType(GLSLType t) {
	switch (t) {
	case GLSLType::SAMPLER1D:
	case GLSLType::SAMPLER1DSHADOW:
	case GLSLType::SAMPLER2D:
	case GLSLType::SAMPLER2DSHADOW:
	case GLSLType::SAMPLER3D:
	case GLSLType::SAMPLERCUBE: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case GLSLType::BOOL:
	case GLSLType::INT:
	case GLSLType::UINT:
	case GLSLType::FLOAT:
	case GLSLType::DOUBLE:
	case GLSLType::BVEC2:
	case GLSLType::BVEC3:
	case GLSLType::BVEC4:
	case GLSLType::IVEC2:
	case GLSLType::IVEC3:
	case GLSLType::IVEC4:
	case GLSLType::UVEC2:
	case GLSLType::UVEC3:
	case GLSLType::UVEC4:
	case GLSLType::VEC2:
	case GLSLType::VEC3:
	case GLSLType::VEC4:
	case GLSLType::DVEC2:
	case GLSLType::DVEC3:
	case GLSLType::DVEC4:
	case GLSLType::MAT2:
	case GLSLType::MAT3:
	case GLSLType::MAT4:
	case GLSLType::STRUCT: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}
}

VkShaderStageFlagBits ShaderPack::_UInt8ToShaderStageFlags(uint8_t flags) {
	int stages = 0x00;
	if (flags & 0b10000000) {
		//vertx
		stages = stages | VK_SHADER_STAGE_VERTEX_BIT;
	}


	if (flags & 0b01000000) {
		//tesc
		stages = stages | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}

	if (flags & 0b00100000) {
		//tese
		stages = stages | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}

	if (flags & 0b00010000) {
		//geom
		stages = stages | VK_SHADER_STAGE_GEOMETRY_BIT;
	}

	if (flags & 0b00001000) {
		//fragment
		stages = stages | VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	if (flags & 0b00000100) {
		//compute
		stages = stages | VK_SHADER_STAGE_COMPUTE_BIT;
	}

	return (VkShaderStageFlagBits)stages;
}