#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

//Using GLFW for simplicity right now, want to be able to use native windows eventually
#include "GLFW/glfw3.h"

class c_instance {
	public:
		c_instance(std::string instance_name);
		VkInstance *get_instance();
		std::string get_instance_name();

	private:
		std::string m_instance_name;
		VkInstance m_instance;
		VkApplicationInfo m_app_info;
		VkInstanceCreateInfo m_instance_create_info;
};