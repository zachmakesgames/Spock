#include "c_instance.h"

c_instance::c_instance(std::string instance_name) : m_instance_name(instance_name), m_instance(nullptr), m_app_info({}), m_instance_create_info({}) {
	

	//Set up application info struct
	this->m_app_info.sType= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	this->m_app_info.pNext= nullptr;
	this->m_app_info.pApplicationName= this->m_instance_name.c_str();
	this->m_app_info.apiVersion= VK_API_VERSION_1_2;
	
	
	//Set up instance create info
	this->m_instance_create_info.sType= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	this->m_instance_create_info.pNext= nullptr;
	this->m_instance_create_info.pApplicationInfo= &this->m_app_info;



	//Set up validation layers for debug builds
#ifdef DEBUG
	uint32_t layer_count= 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	std::vector<VkLayerProperties> available_layers(layer_count);
	std::vector<const char*> validation_layers= {
		"VK_LAYER_KHRONOS_validation"
	};

	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
	for (VkLayerProperties lp : available_layers) {
		if (strcmp(lp.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
			this->m_instance_create_info.enabledLayerCount= 1;
			this->m_instance_create_info.ppEnabledLayerNames= validation_layers.data();
		}
	}
#endif

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//Set up extensions. Only using GLFW right now
	uint32_t glfw_extension_count= 0;
	const char **extensions;
	extensions= glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::cout << "glfw required extensions:" << std::endl;
	for (uint32_t i= 0; i < glfw_extension_count; i++) {
		std::cout << extensions[i] << std::endl;
	}

	this->m_instance_create_info.enabledExtensionCount= glfw_extension_count;
	this->m_instance_create_info.ppEnabledExtensionNames= extensions;
	

	//Create the VkInstance
	VkResult result= vkCreateInstance(&this->m_instance_create_info, nullptr, &this->m_instance);

}


VkInstance* c_instance::get_instance() {
	return &this->m_instance;
}

std::string c_instance::get_instance_name() {
	return this->m_instance_name;
}