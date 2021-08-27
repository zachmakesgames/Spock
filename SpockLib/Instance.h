#pragma once
#include <string>
#include "vulkan/vulkan.h"

//Using GLFW for simplicity right now, want to be able to use native windows eventually
#include "GLFW/glfw3.h"

class Instance {
	public:
		Instance(std::string instance_name);
		VkInstance* GetInstance();

	private:
		std::string instanceName;
		VkInstance instance;
		VkApplicationInfo appInfo;
		VkInstanceCreateInfo instCreateInfo;
};