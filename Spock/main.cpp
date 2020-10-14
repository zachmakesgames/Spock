//This is a place holder main file
//Replace this with whatever you want

#include <iostream>
#include <string>

#include "Graphics/inc/Instance.h"
#include "Graphics/inc/Device.h"
#include "Graphics/inc/View.h"
#include "Graphics/inc/Mesh.h"
#include "Graphics/inc/Buffer.h"
#include "Graphics/inc/DescriptorSet.h"
#include "Graphics/inc/Pipeline.h"


#include "Graphics/VKRenderable.h"
#include "Graphics/VKRenderAgent.h"
#include "Graphics/VKRotatingObject.h"
#include "Utilities/HashTable.h"

#include "ShaderPack_Public.h"

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>

#include "gtc/matrix_transform.hpp"
#include "gtc/matrix_inverse.hpp"

#include <chrono>


int main(int argc, char** argv) {


	VKRenderAgent renderAgent("Vulkan Demo", 800, 800);
	ShaderPack p;


	//Important note: By default the debugger uses solution directory
	//as it's working directory! This means that the application will
	//have access to different files when debugging vs. when running
	//standalone! Keep this in mind becuase this string is written to
	//work in debug mode and NOT when running the standalone .exe
	p.DeSerialize("..\\Resources\\ShaderPacks\\TestPack-6-16.sp");


	//Create two separate descriptor sets so these two objects can have
	//independent values
	DescriptorSet* SaveStationDesSet = renderAgent.ProduceDescriptorSet(p);
	DescriptorSet* CapsuleDesSet = renderAgent.ProduceDescriptorSet(p);

	//The pipeline only cares about the descriptor set layout so we can use
	//either or of the above descriptor sets since they both use the same pipeline
	Pipeline* pl1 = renderAgent.ProducePipeline(SaveStationDesSet, &p);

	Mesh* m = nullptr;

	//These strings suffer the same issue as the one used above for TestPack-6-16.sp!
	Buffer* SaveStationBuffer = renderAgent.ProduceMeshBuffer("..\\Resources\\Models\\SaveStation.obj", m);
	Buffer* CapsuleBuffer = renderAgent.ProduceMeshBuffer("..\\Resources\\Models\\Capsule.obj", m);



	VKRenderable* SaveStation = new VKRenderable(pl1, SaveStationDesSet, SaveStationBuffer);
	VKRotatingObject* object2 = new VKRotatingObject(pl1, CapsuleDesSet, CapsuleBuffer);




	//Setup for delta time counting
	//This is a basic time counting method and is not suitable for
	//industrial strength game engines, however for this demonstration
	//it is more than enough to provde smooth transitions
	auto timeLast = std::chrono::high_resolution_clock::now();
	auto timeNow = std::chrono::high_resolution_clock::now();
	float dt = 0;
	std::chrono::duration<double, std::milli> dms;




	SaveStation->Init();
	object2->Init();
	SaveStation->SetWorldRotation(glm::vec3(0, 1, 0), 180);
	SaveStation->SetWorldLocation(glm::vec3(0, -3, -10));


	object2->scale = glm::vec3(0.7f, 0.7f, 0.7f);

	glm::vec3 color2 = glm::vec3(0, 1, 0);
	CapsuleDesSet->SetUniform("colorBuf", &color2, sizeof(glm::vec3), 0);

	renderAgent.Render(SaveStation);
	renderAgent.Render(object2);
	renderAgent.Display();



	while (!renderAgent.WindowShouldClose()) {

		//glfwPollEvents keeps the window from freezing up in this while loop
		//Since this demo uses glfw we could also make use of the idle callback
		//to update the scene but thats not really necesary for such a simple demo
		glfwPollEvents();

		//First thing to do is update the time delta
		timeNow = std::chrono::high_resolution_clock::now();
		dms = timeNow - timeLast;

		//these both do the same thing, however the second implementation is more verbose
		//dt = (float)dms.count() * 0.001f; 
		dt = dms / std::chrono::seconds(1);

		timeLast = timeNow;



		SaveStation->Update(dt);
		object2->Update(dt);


		renderAgent.Render(SaveStation);
		renderAgent.Render(object2);
		renderAgent.Display();

	}

}