#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"


class Instance {
	public:
		Instance(std::string InstanceName, int SizeX, int SizeY);

		inline bool ShouldClose() {
			return glfwWindowShouldClose(this->Window);
		}

	private:
		int WindowX, WindowY;

		std::string Name;
		GLFWwindow* Window = nullptr;
		VkInstance InstanceVK = {};
		VkSurfaceKHR Surface = {};

		VkApplicationInfo InstanceInfo = {};
		VkInstanceCreateInfo InstanceCreateInfo = {};

		VkExtent2D SurfaceExtent2D = {};

	friend class Pipeline;
	friend class Device;
	friend class View;
};