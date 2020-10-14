#include "Layout.h"
#include "glm_includes.h"

Layout::Layout(std::string RawIn, std::vector<LayoutStage> stages) {
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

size_t Layout::TypeSize(GLSLType t) {
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

std::string Layout::TypeString() {
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

GLSLType Layout::StringToType(std::string s) {
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

std::string Layout::GetStagesString() {
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

void Layout::AddStage(LayoutStage stage) {
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