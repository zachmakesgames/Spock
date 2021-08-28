#include "Instance.h"

Instance::Instance(std::string instance_name) : instanceName(instance_name), instance(nullptr), appInfo({}), instCreateInfo({}) {
	

	//Set up application info struct
	this->appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	this->appInfo.pNext = nullptr;
	this->appInfo.pApplicationName = this->instanceName.c_str();
	this->appInfo.apiVersion = VK_API_VERSION_1_2;
	
	
	//Set up instance create info
	this->instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	this->instCreateInfo.pNext = nullptr;
	this->instCreateInfo.pApplicationInfo = &this->appInfo;



	//Set up validation layers for debug builds
#ifdef DEBUG
	uint32_t layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	std::vector<VkLayerProperties> available_layers(layer_count);
	std::vector<const char*> validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};

	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
	for (VkLayerProperties lp : available_layers) {
		if (strcmp(lp.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
			this->instCreateInfo.enabledLayerCount = 1;
			this->instCreateInfo.ppEnabledLayerNames = validation_layers.data();
		}
	}
#endif

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//Set up extensions. Only using GLFW right now
	uint32_t glfw_extension_count = 0;
	const char** extensions;
	extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::cout << "glfw required extensions:" << std::endl;
	for (uint32_t i = 0; i < glfw_extension_count; ++i) {
		std::cout << extensions[i] << std::endl;
	}

	this->instCreateInfo.enabledExtensionCount = glfw_extension_count;
	this->instCreateInfo.ppEnabledExtensionNames = extensions;
	

	//Create the VkInstance
	VkResult result = vkCreateInstance(&this->instCreateInfo, nullptr, &this->instance);

}


VkInstance* Instance::GetInstance() {
	return &this->instance;
}

std::string Instance::GetInstanceName() {
	return this->instanceName;
}