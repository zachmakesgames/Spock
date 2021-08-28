#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

//Using GLFW for simplicity right now, want to be able to use native windows eventually
#include "GLFW/glfw3.h"

class Instance {
	public:
		Instance(std::string instance_name);
		VkInstance* GetInstance();
		std::string GetInstanceName();

	private:
		std::string instanceName;
		VkInstance instance;
		VkApplicationInfo appInfo;
		VkInstanceCreateInfo instCreateInfo;
};