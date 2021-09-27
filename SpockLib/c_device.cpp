#include "c_device.h"

c_device::c_device(c_instance *instance) : m_instance(instance) {
	
	VkResult result;

	//Enumerate physical devices
	uint32_t device_count;

	vkEnumeratePhysicalDevices(*this->m_instance->get_instance(), &device_count, nullptr);
	if (device_count == 0) {	//Make sure there are actually some graphics devices on the machine
#ifdef DEBUG
		std::cout << "No devices found on this machine!" << std::endl;
#endif
		throw std::exception("No physical graphics devices on this machine");
	}

	VkPhysicalDevice *device_list= new VkPhysicalDevice[device_count];
	vkEnumeratePhysicalDevices(*this->m_instance->get_instance(), &device_count, device_list);


	//Attempt to select the best device.
	///TODO: Add methods to allow user to select device
	
	int discrete_device= -1;
	int virtual_device= -1;

	for (uint32_t i= 0; i < device_count; i++) {
		VkPhysicalDeviceProperties device_properties = {};
		vkGetPhysicalDeviceProperties(device_list[i], &device_properties);
		std::cout << "Device " << i << ": " << device_properties.deviceName;
		
		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			std::cout << " [discrete]" << std::endl;
			discrete_device = i;
		}

		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
			std::cout << " [virtual]" << std::endl;
			virtual_device = i;
		}
	}


	if (discrete_device > -1) {
		this->m_phys_device= device_list[discrete_device];
	}
	else {
		if (virtual_device > -1) {
			this->m_phys_device= device_list[virtual_device];
		}
		else {
#ifdef DEBUG
			std::cout << "Warning! No discrete or virtual devices present! Performance may be poor!" << std::endl;
#endif
			this->m_phys_device= device_list[0];
		}
	}

	
	///TODO: make sure this isn't going to wipe out our device reference
	delete[] device_list;
	


	//Set up the logical device
	uint32_t queue_family_property_count= 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->m_phys_device, &queue_family_property_count, nullptr);

	VkQueueFamilyProperties* queue_properties_list = new VkQueueFamilyProperties[queue_family_property_count];
	vkGetPhysicalDeviceQueueFamilyProperties(this->m_phys_device, &queue_family_property_count, queue_properties_list);

	//Find a queue that supports graphics
	///TODO: Add method to find a queue that supports multiple functions

	//uint32_t graphicsQueueFamilyIndex = -1;
	//uint32_t transferQueueFamilyIndex = -1;
	for (uint32_t i= 0; i < queue_family_property_count; i++) {
		if ((queue_properties_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			m_graphics_queue_familyindex= i;
		}
		if ((queue_properties_list[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
			m_transfer_queue_family_index= i;
		}
	}

	if (m_graphics_queue_familyindex < 0) {
#ifdef DEBUG
		std::cout << "No graphics queue found!" << std::endl;
#endif
		throw std::exception("No graphics queues available");
	}
	if (m_transfer_queue_family_index < 0) {
#ifdef DEBUG
		std::cout << "No transfer queue found!" << std::endl;
#endif
		throw std::exception("No transfer queues available");
	}


	//Get list of supported features for this physical device
	VkPhysicalDeviceFeatures available_features= {};
	vkGetPhysicalDeviceFeatures(this->m_phys_device, &available_features);


	//Set up device queue create info

	float queue_priority= 1.0f;
	this->m_device_queue_create_info.sType= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	this->m_device_queue_create_info.pNext= nullptr;
	this->m_device_queue_create_info.flags= 0;
	this->m_device_queue_create_info.queueFamilyIndex= m_graphics_queue_familyindex;
	this->m_device_queue_create_info.queueCount= 1;
	this->m_device_queue_create_info.pQueuePriorities= &queue_priority;



	uint32_t property_count= 0;
	vkEnumerateDeviceExtensionProperties(this->m_phys_device, NULL, &property_count, NULL);
	std::vector<const char*> ext_names;
	std::vector<VkExtensionProperties> ext_props(property_count);
	vkEnumerateDeviceExtensionProperties(this->m_phys_device, NULL, &property_count, ext_props.data());
	std::cout << "Enumerating supported extensions" << std::endl;
	for (VkExtensionProperties ep : ext_props) {
		std::cout << "Extension name: " << ep.extensionName << std::endl;
	}



	//Need this to actually set up the swap chain
	std::vector<const char*> device_extensions= {
		"VK_KHR_swapchain"
	};

	//Set up the logical device create info
	this->m_device_create_info.sType= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	this->m_device_create_info.pNext= nullptr;
	this->m_device_create_info.queueCreateInfoCount= 1;
	this->m_device_create_info.pQueueCreateInfos= &this->m_device_queue_create_info;
	this->m_device_create_info.pEnabledFeatures= &available_features;
	this->m_device_create_info.enabledExtensionCount= (uint32_t)device_extensions.size();
	this->m_device_create_info.ppEnabledExtensionNames= device_extensions.data();

	//Create the logical device
	result = vkCreateDevice(this->m_phys_device, &this->m_device_create_info, nullptr, &this->m_logical_device);


	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateDevice: " << result << std::endl;
#endif
		throw std::exception("Failed to create vulkan device");
	}

	//Get the selected device queue
	///TODO: Actually pick the right queue index
	vkGetDeviceQueue(this->m_logical_device, m_graphics_queue_familyindex, 0, &this->m_queue);

	if (this->m_queue == nullptr) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the device queue" << std::endl;
#endif
		throw std::exception("Could not get device queue");
	}



	//Create the command pools
	//Need to create one for graphics and one for transfer
	//First find the queue family for graphics and transfer



	this->m_graphics_pool_create_info.sType= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->m_graphics_pool_create_info.pNext= nullptr;
	this->m_graphics_pool_create_info.flags= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	this->m_graphics_pool_create_info.queueFamilyIndex= m_graphics_queue_familyindex;

	result = vkCreateCommandPool(this->m_logical_device, &this->m_graphics_pool_create_info, nullptr, &this->m_graphics_command_pool);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateCommandPool: " << result << std::endl;
#endif
		throw std::exception("Could not create graphics command pool");
	}

	this->m_transfer_pool_create_info.sType= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->m_transfer_pool_create_info.pNext= nullptr;
	this->m_transfer_pool_create_info.flags= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	//this->m_transfer_pool_create_info.queueFamilyIndex= transferQueueFamilyIndex;
	this->m_transfer_pool_create_info.queueFamilyIndex= m_graphics_queue_familyindex;

	result = vkCreateCommandPool(this->m_logical_device, &this->m_transfer_pool_create_info, nullptr, &this->m_transfer_command_pool);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with vkCreateCommandPool: " << result << std::endl;
#endif
		throw std::exception("Could not create transfer command pool");
	}


	//Allocate memory for host memory buffers
	this->m_graphics_command_buffer= new VkCommandBuffer[this->m_graphics_buffer_count];
	this->m_transfer_command_buffer= new VkCommandBuffer[this->m_transfer_buffer_count];



	//Allocate memory for device memory buffers
	this->m_graphics_buffer_alloc_info.sType= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->m_graphics_buffer_alloc_info.pNext= nullptr;
	this->m_graphics_buffer_alloc_info.commandPool= this->m_graphics_command_pool;
	this->m_graphics_buffer_alloc_info.level= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->m_graphics_buffer_alloc_info.commandBufferCount= this->m_graphics_buffer_count;

	result = vkAllocateCommandBuffers(this->m_logical_device, &this->m_graphics_buffer_alloc_info, this->m_graphics_command_buffer);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating graphics buffers: " << result << std::endl;
#endif
		throw std::exception("Could not allocate graphics buffers");
	}

	this->m_transfer_buffer_alloc_info.sType= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	this->m_transfer_buffer_alloc_info.pNext= nullptr;
	this->m_transfer_buffer_alloc_info.commandPool= this->m_transfer_command_pool;
	this->m_transfer_buffer_alloc_info.level= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->m_transfer_buffer_alloc_info.commandBufferCount= this->m_transfer_buffer_count;

	result = vkAllocateCommandBuffers(this->m_logical_device, &this->m_transfer_buffer_alloc_info, this->m_transfer_command_buffer);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating transfer buffer: " << result << std::endl;
#endif
		throw std::exception("Could not allocate transfer buffers");
	}
}


VkPhysicalDevice *c_device::get_physical_device() {
	return &this->m_phys_device;
}

VkDevice *c_device::get_logical_device() {
	return &this->m_logical_device;
}

int c_device::find_memory(VkMemoryPropertyFlagBits memory_flag_bits, uint32_t  memory_type_bits) {
	VkPhysicalDeviceMemoryProperties	memory_properties;
	vkGetPhysicalDeviceMemoryProperties(this->m_phys_device, &memory_properties);
	for (unsigned int i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		VkMemoryType mem_type= memory_properties.memoryTypes[i];
		VkMemoryPropertyFlags property_flags= mem_type.propertyFlags;
		if ((memory_type_bits & (1 << i)) != 0)
		{
			if ((property_flags & memory_flag_bits) != 0)
			{
				return i;
			}
		}
	}
	return -1;
}

VkCommandBuffer *c_device::get_graphics_command_buffers(int *out_count) {
	if (out_count != nullptr) {
		*out_count= this->m_graphics_buffer_count;
	}
	return this->m_graphics_command_buffer;
}

VkCommandBuffer *c_device::get_transfer_command_buffers(int *out_count) {
	if (out_count != nullptr) {
		*out_count= this->m_transfer_buffer_count;
	}
	return this->m_transfer_command_buffer;
}

int c_device::get_graphics_queue_family_index() {
	return this->m_graphics_queue_familyindex;
}

int c_device::get_transfer_queue_family_index() {
	return this->m_transfer_queue_family_index;
}