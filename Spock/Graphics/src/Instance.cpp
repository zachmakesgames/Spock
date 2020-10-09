#include "../inc/Instance.h"
#define DEBUG

Instance::Instance(std::string InstanceName, int SizeX, int SizeY) {


	VkResult result;
	this->Name = InstanceName;
	this->WindowX = SizeX;
	this->WindowY = SizeY;

	//First setup GLFW for the window system
	//This can be swapped out for a different window system if you're crazy I guess
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


	//Set application info here
	this->InstanceInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	this->InstanceInfo.pNext = nullptr;
	this->InstanceInfo.pApplicationName = this->Name.c_str();
	this->InstanceInfo.apiVersion = VK_API_VERSION_1_0;

	//then setup the create info struct
	this->InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	this->InstanceCreateInfo.pNext = nullptr;
	this->InstanceCreateInfo.pApplicationInfo = &this->InstanceInfo;	//this links to InstanceInfo above


	///DEBUG
	///This section adds the validation layer if available to make debugging easier
#ifdef DEBUG
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (VkLayerProperties p : availableLayers) {
		//std::cout << "Available layer: " << p.layerName << std::endl;
		if (strcmp(p.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
			//std::cout << "Found validation layer" << std::endl;
			this->InstanceCreateInfo.enabledLayerCount = 1;
			this->InstanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
	}

#endif


	//get available extensions
	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());
#ifdef DEBUG
	//Only need to do this if debug is enabled
#ifdef VERBOSE
	for (VkExtensionProperties v : availableExtensions) {
		std::cout << "Extension: " << v.extensionName << std::endl;
	}
#endif
#endif

	//Find and add extensions required for GLFW
	uint32_t extensionCount = 0;                                //used to get the extensions that glfw needs to do its thing
	const char** extensions;
	extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
#ifdef DEBUG
	for (int i = 0; i < extensionCount; ++i) {
		//std::cout << "GLFW required extention: " << extensions[i] << std::endl;
	}
#endif
	this->InstanceCreateInfo.enabledExtensionCount = extensionCount;          //add the required extensions to createInfo so Vulkan loads them
	this->InstanceCreateInfo.ppEnabledExtensionNames = extensions;

	result = vkCreateInstance(&this->InstanceCreateInfo, nullptr, &(this->InstanceVK));
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateInstance: " << result << std::endl;
#endif
		throw result;
	}



	//After the extensions are loaded, then create the window and the surface
	this->Window = glfwCreateWindow(this->WindowX, this->WindowY, InstanceName.c_str(), NULL, NULL);
	result = glfwCreateWindowSurface(this->InstanceVK, this->Window, NULL, &(this->Surface));

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with glfwCreateWindowSurface: " << result << std::endl;
#endif
		throw result;
	}
}