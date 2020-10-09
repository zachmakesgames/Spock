#include "../inc/Device.h"
#define DEBUG

Device::Device(Instance* thisInstance) {

	VkResult result;

	//Get the number of physical devices available on this machine
	this->ThisInstance = thisInstance;
	vkEnumeratePhysicalDevices(this->ThisInstance->InstanceVK, &this->DeviceCount, nullptr);
	if (this->DeviceCount == 0) {	//Make sure there are actually some graphics devices on the machine
#ifdef DEBUG
		std::cout << "No devices found on this machine!" << std::endl;
#endif
		throw - 1;
	}


	//setup the list of devices and then enumerate them into DeviceList
	this->DeviceList = new VkPhysicalDevice[this->DeviceCount];
	vkEnumeratePhysicalDevices(this->ThisInstance->InstanceVK, &this->DeviceCount, this->DeviceList);

	//Select a device from the list.
	//Prefer discrete devices but should impliment some feature for users to be able
	//to select a device in settings
	int discreteDevice = -1;
	int virtualDevice = -1;
	for (int i = 0; i < this->DeviceCount; ++i) {
		VkPhysicalDeviceProperties deviceProps = {};
		vkGetPhysicalDeviceProperties(this->DeviceList[i], &deviceProps);
		if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) discreteDevice = i;
		if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) virtualDevice = i;
	}

	if (discreteDevice > -1) {
		this->DeviceVK = this->DeviceList[discreteDevice];
	}
	else {
		if (virtualDevice > -1) {
			this->DeviceVK = this->DeviceList[virtualDevice];
		}
		else {
#ifdef DEBUG
			std::cout << "Warning! No discrete or virtual devices present! Performance may be poor!" << std::endl;
#endif
			this->DeviceVK = this->DeviceList[0];
		}
	}

	delete[] this->DeviceList;



	//Setup the logical device here
	//get a graphics queue for this device
	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->DeviceVK, &queueFamilyPropertyCount, nullptr);
	VkQueueFamilyProperties* queueProperties = new VkQueueFamilyProperties[queueFamilyPropertyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(this->DeviceVK, &queueFamilyPropertyCount, queueProperties);

	//Look for the queue that supports graphics
	for (int i = 0; i < queueFamilyPropertyCount; ++i) {
		if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			this->GraphicsQueue = i;
		}
		/* May be useful in the future
		if ((queueProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) {
			std::cout << "Queue " << i << " supports compute" << std::endl;
		}
		if ((queueProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
			std::cout << "Queue " << i << " supports transfers" << std::endl;
		}*/
	}

	if (this->GraphicsQueue == -1) {
#ifdef DEBUG
		std::cout << "No graphics queue found!" << std::endl;
#endif
		throw -1;	//Why do I do this to myself?
	}

	//Get supported features from the device
	VkPhysicalDeviceFeatures availableFeatures = {};
	vkGetPhysicalDeviceFeatures(this->DeviceVK, &availableFeatures);

	float priority = 1.0f;
	this->DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	this->DeviceQueueCreateInfo.pNext = nullptr;
	this->DeviceQueueCreateInfo.flags = 0;
	this->DeviceQueueCreateInfo.queueFamilyIndex = this->GraphicsQueue;	//The queue family index found above
	this->DeviceQueueCreateInfo.queueCount = 1;
	this->DeviceQueueCreateInfo.pQueuePriorities = &priority;

	//Need this to actually set up the swap chain
	std::vector<const char*> deviceExtensions = {
		"VK_KHR_swapchain"
	};

	//Set up the device create info for the logical device
	this->LogicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	this->LogicalDeviceCreateInfo.pNext = nullptr;
	this->LogicalDeviceCreateInfo.queueCreateInfoCount = 1;
	this->LogicalDeviceCreateInfo.pQueueCreateInfos = &this->DeviceQueueCreateInfo;
	this->LogicalDeviceCreateInfo.pEnabledFeatures = &availableFeatures;
	this->LogicalDeviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
	this->LogicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	//create the logical device
	result = vkCreateDevice(this->DeviceVK, &this->LogicalDeviceCreateInfo, nullptr, &this->LogicalDevice);


	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateDevice: " << result << std::endl;
#endif
		throw result;
	}

	//Then get the device queue
	vkGetDeviceQueue(this->LogicalDevice, 0, 0, &this->DeviceQueue); ///TODO: Actually pick the right queue family index

	if (this->DeviceQueue == nullptr) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the device queue" << std::endl;
#endif
		throw -1;
	}

	//Create the command pools
	//Need to create one for graphics and one for transfer


	//First find the queue family for graphics and transfer
	int GraphicsFamily = -1;
	int TransferFamily = -1;

	uint32_t count = -1;
	vkGetPhysicalDeviceQueueFamilyProperties(this->DeviceVK, &count, nullptr);
	VkQueueFamilyProperties* vqfp = new VkQueueFamilyProperties[count];
	vkGetPhysicalDeviceQueueFamilyProperties(this->DeviceVK, &count, vqfp);
	for (unsigned int i = 0; i < count; i++)
	{
		VkQueueFlags flags = vqfp[i].queueFlags;
		if ((flags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			delete[] vqfp;
			vqfp = nullptr;
			GraphicsFamily = i;
			break;
		}
	}
	if(vqfp != nullptr) delete[] vqfp;

	count = -1;
	vkGetPhysicalDeviceQueueFamilyProperties(this->DeviceVK, &count, nullptr);
	vqfp = new VkQueueFamilyProperties[count];
	vkGetPhysicalDeviceQueueFamilyProperties(this->DeviceVK, &count, vqfp);
	for (unsigned int i = 0; i < count; i++)
	{
		if ((vqfp[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
		{
			delete[] vqfp;
			vqfp = nullptr;
			TransferFamily = i;
			break;
		}
	}
	if (vqfp != nullptr) delete[] vqfp;

	if (GraphicsFamily == -1) {
#ifdef DEBUG
		std::cout << "Vulkan could not find any device queue families with graphics capabilities!" << std::endl;
#endif
		throw -1;
	}
	if (TransferFamily == -1) {
#ifdef DEBUG
		std::cout << "Vulkan could not find any device queue families with transfer capabilities!" << std::endl;
#endif
		throw - 1;
	}

	this->GraphicsPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->GraphicsPoolCreateInfo.pNext = nullptr;
	this->GraphicsPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	this->GraphicsPoolCreateInfo.queueFamilyIndex = GraphicsFamily;

	result = vkCreateCommandPool(this->LogicalDevice, &this->GraphicsPoolCreateInfo, nullptr, &this->GraphicsPool);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateCommandPool: " << result << std::endl;
#endif
		throw result;
	}

	this->TransferPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->TransferPoolCreateInfo.pNext = nullptr;
	this->TransferPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	this->TransferPoolCreateInfo.queueFamilyIndex = TransferFamily;

	result = vkCreateCommandPool(this->LogicalDevice, &this->TransferPoolCreateInfo, nullptr, &this->TransferPool);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateCommandPool: " << result << std::endl;
#endif
		throw result;
	}


	//Then allocate the command and transfer buffers

	this->GraphicsBuffers = new VkCommandBuffer[this->GraphicsBufferCount];
	this->TransferBuffers = new VkCommandBuffer[this->TransferBufferCount];

	//Graphics command buffer
	this->GraphicsBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->GraphicsBufferAllocateInfo.pNext = nullptr;
	this->GraphicsBufferAllocateInfo.commandPool = this->GraphicsPool;
	this->GraphicsBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->GraphicsBufferAllocateInfo.commandBufferCount = this->GraphicsBufferCount;

	result = vkAllocateCommandBuffers(this->LogicalDevice, &this->GraphicsBufferAllocateInfo, this->GraphicsBuffers);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating graphics buffer: " << result << std::endl;
#endif
		throw result;
	}

	//Graphics Transfer buffer
	this->TransferBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->TransferBufferAllocateInfo.pNext = nullptr;
	this->TransferBufferAllocateInfo.commandPool = this->TransferPool;
	this->TransferBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->TransferBufferAllocateInfo.commandBufferCount = this->TransferBufferCount;

	result = vkAllocateCommandBuffers(this->LogicalDevice, &this->TransferBufferAllocateInfo, this->TransferBuffers);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating transfer buffer: " << result << std::endl;
#endif
		throw result;
	}


	//Also need to check if the surface created by the instance is valid
	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->DeviceVK, this->ThisInstance->Surface, &surfaceCapabilities);

	//May need this later
	///NOTE: This call reaches back in the flow to set something in the instance
	this->ThisInstance->SurfaceExtent2D = surfaceCapabilities.currentExtent;

	//Check surface support here
	VkBool32 surfaceSupport;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(this->DeviceVK, this->GraphicsQueue, this->ThisInstance->Surface, &surfaceSupport);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting surface support results: " << result << std::endl;
#endif
		throw result;
	}


	if (surfaceSupport == VK_TRUE) {
#ifdef DEBUG
		std::cout << "Surface is supported by the queue" << std::endl;
#endif
	}
	else {
		//should really do something drastic here I guess
#ifdef DEBUG
		std::cout << "Surface is NOT supported by the queue!" << std::endl;
#endif
		throw -1;
	}
	
}


Device::~Device(){}

VkPhysicalDevice Device::GetPhysicalDevice() {
	return this->DeviceVK;
}

VkDevice Device::GetLogicalDevice() {
	return this->LogicalDevice;
}

VkSurfaceKHR Device::GetSurface() {
	return this->ThisInstance->Surface;
}

VkExtent2D Device::GetSurfaceExtent() {
	return this->ThisInstance->SurfaceExtent2D;
}

uint32_t Device::GetGraphicsQueueIndex() {
	return this->GraphicsQueue;
}


int Device::FindMemory(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
	VkPhysicalDeviceMemoryProperties	memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(this->DeviceVK, &memoryProperties);
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
	//throw  std::runtime_error( "Could not find given memory flag and type" );
	return  -1;
}

VkCommandBuffer* Device::GetCommandBuffers() {
	return this->GraphicsBuffers;
}