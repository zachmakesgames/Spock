#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>



#include "vec3.hpp"
#include "vec2.hpp"
#include "mat4x4.hpp"

#include "Device.h"


class Mesh
{


private:
	glm::vec3 importScale = glm::vec3(1, 1, 1);
	glm::vec3 importTransform = glm::vec3(0, 0, 0);


	uint64_t verts = 0;
	uint64_t dataSize = 0;
	uint64_t facesCount = 0;
	bool facesLoaded = false;

	float* data = nullptr;

	//GLuint vertBuffer;

	std::string modelName = "";

	Device* ThisDevice = nullptr;
	
	
public:
	Mesh(Device* d);
	~Mesh();

	void Load(std::string objFileName);
	void Unload();
	void Draw();

	float* GetRaw(int& size);
};

