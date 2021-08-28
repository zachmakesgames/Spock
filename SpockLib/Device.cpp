#include "Device.h"

Device::Device(Instance* instance) : instance(instance) {
	
	VkResult result;

	//Enumerate physical devices
	uint32_t device_count;

	vkEnumeratePhysicalDevices(*this->instance->GetInstance(), &device_count, nullptr);
	if (device_count == 0) {	//Make sure there are actually some graphics devices on the machine
#ifdef DEBUG
		std::cout << "No devices found on this machine!" << std::endl;
#endif
		throw std::exception("No physical graphics devices on this machine");
	}

	VkPhysicalDevice* device_list = new VkPhysicalDevice[device_count];
	vkEnumeratePhysicalDevices(*this->instance->GetInstance(), &device_count, device_list);


	//Attempt to select the best device.
	///TODO: Add methods to allow user to select device
	
	int discrete_device = -1;
	int virtual_device = -1;

	for (uint32_t i = 0; i < device_count; ++i) {
		VkPhysicalDeviceProperties device_properties = {};
		vkGetPhysicalDeviceProperties(device_list[i], &device_properties);
		
		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			discrete_device = i;
		}

		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
			virtual_device = i;
		}
	}


	if (discrete_device > -1) {
		this->physDevice = device_list[discrete_device];
	}
	else {
		if (virtual_device > -1) {
			this->physDevice = device_list[virtual_device];
		}
		else {
#ifdef DEBUG
			std::cout << "Warning! No discrete or virtual devices present! Performance may be poor!" << std::endl;
#endif
			this->physDevice = device_list[0];
		}
	}

	
	///TODO: make sure this isn't going to wipe out our device reference
	delete[] device_list;



	//Set up the logical device
	uint32_t queue_family_property_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->physDevice, &queue_family_property_count, nullptr);

	VkQueueFamilyProperties* queue_properties_list = new VkQueueFamilyProperties[queue_family_property_count];
	vkGetPhysicalDeviceQueueFamilyProperties(this->physDevice, &queue_family_property_count, queue_properties_list);

	//Find a queue that supports graphics
	///TODO: Add method to find a queue that supports multiple functions

	//uint32_t graphicsQueueFamilyIndex = -1;
	//uint32_t transferQueueFamilyIndex = -1;
	for (uint32_t i = 0; i < queue_family_property_count; ++i) {
		if ((queue_properties_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			graphicsQueueFamilyIndex = i;
		}
		if ((queue_properties_list[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
			transferQueueFamilyIndex = i;
		}
	}

	if (graphicsQueueFamilyIndex < 0) {
#ifdef DEBUG
		std::cout << "No graphics queue found!" << std::endl;
#endif
		throw std::exception("No graphics queues available");
	}
	if (transferQueueFamilyIndex < 0) {
#ifdef DEBUG
		std::cout << "No transfer queue found!" << std::endl;
#endif
		throw std::exception("No transfer queues available");
	}


	//Get list of supported features for this physical device
	VkPhysicalDeviceFeatures available_features = {};
	vkGetPhysicalDeviceFeatures(this->physDevice, &available_features);


	//Set up device queue create info

	float queue_priority = 1.0f;
	this->deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	this->deviceQueueCreateInfo.pNext = nullptr;
	this->deviceQueueCreateInfo.flags = 0;
	this->deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	this->deviceQueueCreateInfo.queueCount = 1;
	this->deviceQueueCreateInfo.pQueuePriorities = &queue_priority;



	uint32_t property_count = 0;
	vkEnumerateDeviceExtensionProperties(this->physDevice, NULL, &property_count, NULL);
	std::vector<const char*> ext_names;
	std::vector<VkExtensionProperties> ext_props(property_count);
	vkEnumerateDeviceExtensionProperties(this->physDevice, NULL, &property_count, ext_props.data());
	std::cout << "Enumerating supported extensions" << std::endl;
	for (VkExtensionProperties ep : ext_props) {
		std::cout << "Extension name: " << ep.extensionName << std::endl;
	}



	//Need this to actually set up the swap chain
	std::vector<const char*> device_extensions = {
		"VK_KHR_swapchain"
	};

	//Set up the logical device create info
	this->deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	this->deviceCreateInfo.pNext = nullptr;
	this->deviceCreateInfo.queueCreateInfoCount = 1;
	this->deviceCreateInfo.pQueueCreateInfos = &this->deviceQueueCreateInfo;
	this->deviceCreateInfo.pEnabledFeatures = &available_features;
	this->deviceCreateInfo.enabledExtensionCount = (uint32_t)device_extensions.size();
	this->deviceCreateInfo.ppEnabledExtensionNames = device_extensions.data();

	//Create the logical device
	result = vkCreateDevice(this->physDevice, &this->deviceCreateInfo, nullptr, &this->logicalDevice);


	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateDevice: " << result << std::endl;
#endif
		throw std::exception("Failed to create vulkan device");
	}

	//Get the selected device queue
	///TODO: Actually pick the right queue index
	vkGetDeviceQueue(this->logicalDevice, graphicsQueueFamilyIndex, 0, &this->queue);

	if (this->queue == nullptr) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the device queue" << std::endl;
#endif
		throw std::exception("Could not get device queue");
	}



	//Create the command pools
	//Need to create one for graphics and one for transfer
	//First find the queue family for graphics and transfer



	this->gfxPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->gfxPoolCreateInfo.pNext = nullptr;
	this->gfxPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	this->gfxPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

	result = vkCreateCommandPool(this->logicalDevice, &this->gfxPoolCreateInfo, nullptr, &this->gfxCmdPool);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateCommandPool: " << result << std::endl;
#endif
		throw std::exception("Could not create graphics command pool");
	}

	this->transferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->transferCreateInfo.pNext = nullptr;
	this->transferCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	//this->transferCreateInfo.queueFamilyIndex = transferQueueFamilyIndex;
	this->transferCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

	result = vkCreateCommandPool(this->logicalDevice, &this->transferCreateInfo, nullptr, &this->transferCmdPool);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateCommandPool: " << result << std::endl;
#endif
		throw std::exception("Could not create transfer command pool");
	}


	//Allocate memory for host memory buffers
	this->gfxCmdBuffers = new VkCommandBuffer[this->graphicsBufferCount];
	this->transferCmdBuffer = new VkCommandBuffer[this->transferBufferCount];



	//Allocate memory for device memory buffers
	this->gfxBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->gfxBufferAllocInfo.pNext = nullptr;
	this->gfxBufferAllocInfo.commandPool = this->gfxCmdPool;
	this->gfxBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->gfxBufferAllocInfo.commandBufferCount = this->graphicsBufferCount;

	result = vkAllocateCommandBuffers(this->logicalDevice, &this->gfxBufferAllocInfo, this->gfxCmdBuffers);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating graphics buffers: " << result << std::endl;
#endif
		throw std::exception("Could not allocate graphics buffers");
	}

	this->transferBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->transferBufferAllocInfo.pNext = nullptr;
	this->transferBufferAllocInfo.commandPool = this->transferCmdPool;
	this->transferBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->transferBufferAllocInfo.commandBufferCount = this->transferBufferCount;

	result = vkAllocateCommandBuffers(this->logicalDevice, &this->transferBufferAllocInfo, this->transferCmdBuffer);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating transfer buffer: " << result << std::endl;
#endif
		throw std::exception("Could not allocate transfer buffers");
	}
}


VkPhysicalDevice* Device::GetPhysDevice() {
	return &this->physDevice;
}

VkDevice* Device::GetLogicalDevice() {
	return &this->logicalDevice;
}

int Device::FindMemory(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
	VkPhysicalDeviceMemoryProperties	memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(this->physDevice, &memoryProperties);
	for (unsigned int i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		VkMemoryType memType = memoryProperties.memoryTypes[i];
		VkMemoryPropertyFlags propertyFlags = memType.propertyFlags;
		if ((memoryTypeBits & (1 << i)) != 0)
		{
			if ((propertyFlags & memoryFlagBits) != 0)
			{
				return i;
			}
		}
	}
	return -1;
}

VkCommandBuffer* Device::GetGfxCmdBuffers(int* outCount) {
	if (outCount != nullptr) {
		*outCount = this->graphicsBufferCount;
	}
	return this->gfxCmdBuffers;
}

VkCommandBuffer* Device::GetTransferCmdBuffer() {
	return this->transferCmdBuffer;
}

int Device::GetGfxQueueFamilyIndex() {
	return this->graphicsQueueFamilyIndex;
}

int Device::GetTransferQueueFamilyIndex() {
	return this->transferQueueFamilyIndex;
}