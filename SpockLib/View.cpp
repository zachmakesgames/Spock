#include "View.h"

View::View(Instance* instance, Device* device, int x, int y) :
	instance(instance), device(device), closingCallback(nullptr), resizeCallback(nullptr),
	extent2D({})
{
	
	VkResult result;
	
	//Set up the window


	this->extent2D.height = y;
	this->extent2D.width = x;

	this->window = glfwCreateWindow(x, y, instance->GetInstanceName().c_str(), NULL, NULL);
	result = glfwCreateWindowSurface(*this->instance->GetInstance(), this->window, NULL, &this->windowSurface);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with glfwCreateWindowSurface: " << result << std::endl;
#endif
		throw std::exception("Could not create GLFW window surface");
	}



	//Check for surface support, vulkan will complain if we dont
	VkBool32 supported_surface = false;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(*this->device->GetPhysDevice(), this->device->GetGfxQueueFamilyIndex(), this->windowSurface, &supported_surface);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan could not determine if surface is supported: " << result << std::endl;
#endif
		throw std::exception("Could not create GLFW window surface");
	}

	if (!supported_surface) {
#ifdef DEBUG
		std::cout << "SURFACE IS NOT SUPPORTED!!!!" << std::endl;
#endif
		throw std::exception("GLFW surface is not supported by this device");
	}

	
	//Check which formats are available
	//First find out how many there are
	uint32_t format_count = -1;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*this->device->GetPhysDevice(), this->windowSurface, &format_count, nullptr);
	VkSurfaceFormatKHR* supported_formats = new VkSurfaceFormatKHR[format_count];



	//Then populate the formats
	vkGetPhysicalDeviceSurfaceFormatsKHR(*this->device->GetPhysDevice(), this->windowSurface, &format_count, supported_formats);
#ifdef DEBUG

	std::cout << "Found " << format_count << " supported formats" << std::endl;
	for (int i = 0; i < format_count; ++i) {
		switch (supported_formats[i].format) {
		case VK_FORMAT_R8G8B8A8_UNORM: std::cout << "Format: VK_FORMAT_R8G8B8A8_UNORM" << std::endl;
			break;
		case VK_FORMAT_R8G8B8A8_SNORM: std::cout << "Format: VK_FORMAT_R8G8B8A8_SNORM" << std::endl;
			break;
		case VK_FORMAT_R8G8B8A8_USCALED: std::cout << "Format: VK_FORMAT_R8G8B8A8_USCALED" << std::endl;
			break;
		case VK_FORMAT_R8G8B8A8_SSCALED: std::cout << "Format: VK_FORMAT_R8G8B8A8_SSCALED" << std::endl;
			break;
		case VK_FORMAT_R8G8B8A8_UINT: std::cout << "Format: VK_FORMAT_R8G8B8A8_UINT" << std::endl;
			break;
		case VK_FORMAT_R8G8B8A8_SINT: std::cout << "Format: VK_FORMAT_R8G8B8A8_SINT" << std::endl;
			break;
		case VK_FORMAT_R8G8B8A8_SRGB: std::cout << "Format: VK_FORMAT_R8G8B8A8_SRGB" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_UNORM: std::cout << "Format: VK_FORMAT_B8G8R8A8_UNORM" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_SNORM: std::cout << "Format: VK_FORMAT_B8G8R8A8_SNORM" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_USCALED: std::cout << "Format: VK_FORMAT_B8G8R8A8_USCALED" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_SSCALED: std::cout << "Format: VK_FORMAT_B8G8R8A8_SSCALED" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_UINT: std::cout << "Format: VK_FORMAT_B8G8R8A8_UINT" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_SINT: std::cout << "Format: VK_FORMAT_B8G8R8A8_SINT" << std::endl;
			break;
		case VK_FORMAT_B8G8R8A8_SRGB: std::cout << "Format: VK_FORMAT_B8G8R8A8_SRGB" << std::endl;
			break;
		}
		switch (supported_formats[i].colorSpace) {
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: std::cout << "Colorspace: VK_COLOR_SPACE_SRGB_NONLINEAR_KHR" << std::endl;
			break;
		case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_DCI_P3_LINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_DCI_P3_LINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_BT709_LINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_BT709_LINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_BT709_NONLINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_BT709_NONLINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_BT2020_LINEAR_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_BT2020_LINEAR_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_HDR10_ST2084_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_HDR10_ST2084_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_DOLBYVISION_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_DOLBYVISION_EXT" << std::endl;
			break;
		case VK_COLOR_SPACE_HDR10_HLG_EXT: std::cout << "Colorspace: VK_COLOR_SPACE_HDR10_HLG_EXT" << std::endl;
			break;

		}
	}
#endif



	//Check which present modes are available
	uint32_t present_mode_count = -1;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*this->device->GetPhysDevice(), this->windowSurface, &present_mode_count, nullptr);
	VkPresentModeKHR* present_modes = new VkPresentModeKHR[present_mode_count];

	//Then populate the present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(*this->device->GetPhysDevice(), this->windowSurface, &present_mode_count, present_modes);
#ifdef DEBUG
	std::cout << "Found " << present_mode_count << " presentation modes" << std::endl;
	for (int i = 0; i < present_mode_count; ++i) {
		switch (present_modes[i]) {
		case VK_PRESENT_MODE_IMMEDIATE_KHR: std::cout << "VK_PRESENT_MODE_IMMEDIATE_KHR" << std::endl;
			break;
		case VK_PRESENT_MODE_MAILBOX_KHR: std::cout << "VK_PRESENT_MODE_MAILBOX_KHR" << std::endl;
			break;
		case VK_PRESENT_MODE_FIFO_KHR: std::cout << "VK_PRESENT_MODE_FIFO_KHR" << std::endl;
			break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR: std::cout << "VK_PRESENT_MODE_FIFO_RELAXED_KHR" << std::endl;
			break;
		case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR: std::cout << "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR" << std::endl;
			break;
		case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: std::cout << "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR" << std::endl;
			break;
		}
	}
#endif


	

	//Set up the swap chain creation info
	///TODO: set this up more dynamically
	this->swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	this->swapchainCreateInfo.pNext = nullptr;
	this->swapchainCreateInfo.flags = 0;
	this->swapchainCreateInfo.surface = this->windowSurface;
	this->swapchainCreateInfo.minImageCount = this->swapchainImageCount;
	//VkFormat selectedFormat = this->GetImageFormat();
	this->swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	this->swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;// this->GetColorSpaceFromFormat(selectedFormat);
	this->swapchainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;	///TODO: Add method to use different present modes
	this->swapchainCreateInfo.imageExtent = this->extent2D;
	this->swapchainCreateInfo.imageArrayLayers = 1;
	this->swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	this->swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	this->swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	this->swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	this->swapchainCreateInfo.queueFamilyIndexCount = this->device->GetGfxQueueFamilyIndex();
	this->swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	this->swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	this->swapchainCreateInfo.clipped = VK_TRUE;



	result = vkCreateSwapchainKHR(*this->device->GetLogicalDevice(), &this->swapchainCreateInfo, nullptr, &this->swapchain);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the swapchain: " << result << std::endl;
#endif
		throw std::exception("Could not create swapchain");
	}


	//Next get the swapchain images
	result = vkGetSwapchainImagesKHR(*this->device->GetLogicalDevice(), this->swapchain, &this->swapchainImageCount, nullptr);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the present image count: " << result << std::endl;
#endif
		throw std::exception("Could not get swapchain images");
	}
	if (this->presentImageCount < this->swapchainImageCount) {
#ifdef DEBUG
		std::cout << "Did not get the number of requested swap chain images!" << std::endl;
#endif
		throw std::exception("Swapchain/present image count mismatch");
	}


	this->presentImages = new VkImage[this->presentImageCount];
	result = vkGetSwapchainImagesKHR(*this->device->GetLogicalDevice(), this->swapchain, &this->presentImageCount, this->presentImages);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the present images: " << result << std::endl;
#endif
		throw std::exception("Error creating swapchain images");
	}


	//Set up Present Views
	this->presentImageViews = new VkImageView[this->presentImageCount];
	this->presentImageViewCreateInfo = new VkImageViewCreateInfo[this->presentImageCount];
	for (int i = 0; i < this->presentImageCount; ++i) {
		this->presentImageViewCreateInfo[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		this->presentImageViewCreateInfo[i].pNext = nullptr;
		this->presentImageViewCreateInfo[i].flags = 0;
		this->presentImageViewCreateInfo[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
		this->presentImageViewCreateInfo[i].format = VK_FORMAT_B8G8R8A8_SRGB;	//need to correlate this to the available formats above?
		//this->presentImageViewCreateInfo[i].format = this->GetImageFormat();	//There, now its linked to the available formats
		this->presentImageViewCreateInfo[i].components.r = VK_COMPONENT_SWIZZLE_R;
		this->presentImageViewCreateInfo[i].components.g = VK_COMPONENT_SWIZZLE_G;
		this->presentImageViewCreateInfo[i].components.b = VK_COMPONENT_SWIZZLE_B;
		this->presentImageViewCreateInfo[i].components.a = VK_COMPONENT_SWIZZLE_A;
		this->presentImageViewCreateInfo[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		this->presentImageViewCreateInfo[i].subresourceRange.baseMipLevel = 0;
		this->presentImageViewCreateInfo[i].subresourceRange.levelCount = 1;
		this->presentImageViewCreateInfo[i].subresourceRange.baseArrayLayer = 0;
		this->presentImageViewCreateInfo[i].subresourceRange.layerCount = 1;
		this->presentImageViewCreateInfo[i].image = this->presentImages[i];

		result = vkCreateImageView(*this->device->GetLogicalDevice(), &this->presentImageViewCreateInfo[i], nullptr, &this->presentImageViews[i]);

		if (result != VK_SUCCESS) {
#ifdef DEBUG
			std::cout << "Vulkan encountered an error creating image view " << i << ": " << result << std::endl;
#endif
			throw std::exception("Could not create image view");
		}
	}





	//Set up the depth stencil
	this->extent3D = { this->extent2D.width, this->extent2D.height, 1 };

	this->imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	this->imageCreateInfo.pNext = nullptr;
	this->imageCreateInfo.flags = 0;
	this->imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	this->imageCreateInfo.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	this->imageCreateInfo.extent = this->extent3D;
	this->imageCreateInfo.mipLevels = 1;
	this->imageCreateInfo.arrayLayers = 1;
	this->imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	this->imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	this->imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	this->imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	this->imageCreateInfo.queueFamilyIndexCount = 0;
	this->imageCreateInfo.pQueueFamilyIndices = nullptr;
	this->imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	this->depthStencilImage = {};
	result = vkCreateImage(*this->device->GetLogicalDevice(), &this->imageCreateInfo, nullptr, &this->depthStencilImage);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the depth stencil: " << result << std::endl;
#endif
		throw std::exception("Could not create depth stencil image");
	}


	//Get information about what memory requirements the depth stencil needs
	vkGetImageMemoryRequirements(*this->device->GetLogicalDevice(), this->depthStencilImage, &this->stencilRequirements);

	VkPhysicalDeviceMemoryProperties memProperties = {};	///TODO: Do this in the Device class earlier and save for later use
	vkGetPhysicalDeviceMemoryProperties(*this->device->GetPhysDevice(), &memProperties);




	//Find memory that is device local
	uint32_t memoryIndex = -1;
	for (unsigned int i = 0; i < memProperties.memoryTypeCount; ++i) {
		VkMemoryType memType = memProperties.memoryTypes[i];
		VkMemoryPropertyFlags memFlags = memType.propertyFlags;
		if ((this->stencilRequirements.memoryTypeBits & (1 << i)) != 0) {
			if ((memFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) {
				memoryIndex = i;
				break;
			}
		}
	}
	if (memoryIndex < 0) {
		throw std::exception("No device memory??!!");
	}

	//Set up the memory allocation info struct
	this->stencilMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	this->stencilMemoryAllocInfo.pNext = nullptr;
	this->stencilMemoryAllocInfo.allocationSize = this->stencilRequirements.size;
	this->stencilMemoryAllocInfo.memoryTypeIndex = memoryIndex;

	//Then allocate the memory
	this->stencilMemory = new VkDeviceMemory();
	result = vkAllocateMemory(*this->device->GetLogicalDevice(), &this->stencilMemoryAllocInfo, nullptr, this->stencilMemory);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating memory for the depth stencil: " << result << std::endl;
#endif
		throw std::exception("Could not allocate memory for depth stencil");
	}

	//And bind the memory
	result = vkBindImageMemory(*this->device->GetLogicalDevice(), depthStencilImage, *this->stencilMemory, 0);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error binding the depth stencil image memory: " << result << std::endl;
#endif
		throw std::exception("Could not bind stnecil image memory");
	}


	//
	//Then set up the image view
	this->imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	this->imageViewCreateInfo.pNext = nullptr;
	this->imageViewCreateInfo.flags = 0;
	this->imageViewCreateInfo.image = this->depthStencilImage;
	this->imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	this->imageViewCreateInfo.format = this->imageCreateInfo.format;
	this->imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	this->imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	this->imageViewCreateInfo.subresourceRange.levelCount = 1;
	this->imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	this->imageViewCreateInfo.subresourceRange.layerCount = 1;

	this->imageView = new VkImageView();
	result = vkCreateImageView(*this->device->GetLogicalDevice(), &this->imageViewCreateInfo, nullptr, this->imageView);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the image view: " << result << std::endl;
#endif
		throw std::exception("Could not create image view");
	}



	//
	//Then set up render passes
	//copied this from sample code developed by Dr. Mike Bailey at Oregon State University
	//Doesn't really need to be changed yet so I'm attributing the original author because reasons

	// need 2 - one for the color and one for the depth/stencil
	VkAttachmentDescription				vad[2];
	//vad[0].format = VK_FORMAT_B8G8R8A8_UNORM;
	vad[0].format = VK_FORMAT_B8G8R8A8_SRGB;
	//vad[0].format = this->GetImageFormat();
	vad[0].samples = VK_SAMPLE_COUNT_1_BIT;
	vad[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	vad[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vad[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	vad[0].flags = 0;
	//vad[0].flags = VK_ATTACHMENT_DESCRIPTION_MAT_ALIAS_BIT;

	vad[1].format = VK_FORMAT_D32_SFLOAT_S8_UINT; //Note this is not using GetImageFormat because the depth format is different
	vad[1].samples = VK_SAMPLE_COUNT_1_BIT;
	vad[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vad[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	vad[1].flags = 0;

	VkAttachmentReference				colorReference;
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference				depthReference;
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription				vsd;
	vsd.flags = 0;
	vsd.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	vsd.inputAttachmentCount = 0;
	vsd.pInputAttachments = (VkAttachmentReference*)nullptr;
	vsd.colorAttachmentCount = 1;
	vsd.pColorAttachments = &colorReference;
	vsd.pResolveAttachments = (VkAttachmentReference*)nullptr;
	vsd.pDepthStencilAttachment = &depthReference;
	vsd.preserveAttachmentCount = 0;
	vsd.pPreserveAttachments = (uint32_t*)nullptr;





	this->renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	this->renderPassCreateInfo.pNext = nullptr;
	this->renderPassCreateInfo.flags = 0;
	this->renderPassCreateInfo.attachmentCount = 2;
	this->renderPassCreateInfo.pAttachments = vad;
	this->renderPassCreateInfo.subpassCount = 1;
	this->renderPassCreateInfo.pSubpasses = &vsd;
	this->renderPassCreateInfo.dependencyCount = 0;
	this->renderPassCreateInfo.pDependencies = nullptr;

	this->renderPasses = new VkRenderPass();
	result = vkCreateRenderPass(*this->device->GetLogicalDevice(), &this->renderPassCreateInfo, nullptr, this->renderPasses);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the render pass: " << result << std::endl;
#endif
		throw std::exception("Could not create render passes");
	}


	//
	//Finally set up the frame buffer
	this->framebuffers = new VkFramebuffer[this->framebufferCount];
	this->framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	this->framebufferCreateInfo.pNext = nullptr;
	this->framebufferCreateInfo.flags = 0;
	this->framebufferCreateInfo.renderPass = *this->renderPasses;
	this->framebufferCreateInfo.attachmentCount = 2;
	this->framebufferCreateInfo.pAttachments = framebufferAttachments;
	this->framebufferCreateInfo.width = this->extent2D.width;
	this->framebufferCreateInfo.height = this->extent2D.height;
	this->framebufferCreateInfo.layers = 1;

	this->framebufferAttachments[0] = this->presentImageViews[0];
	this->framebufferAttachments[1] = *this->imageView;
	result = vkCreateFramebuffer(*this->device->GetLogicalDevice(), &this->framebufferCreateInfo, nullptr, &this->framebuffers[0]);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the first frame buffer: " << result << std::endl;
#endif
		throw std::exception("Could not create first frame buffer");
	}



	this->framebufferAttachments[0] = this->presentImageViews[1];
	this->framebufferAttachments[1] = *this->imageView;
	result = vkCreateFramebuffer(*this->device->GetLogicalDevice(), &this->framebufferCreateInfo, nullptr, &this->framebuffers[1]);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the second frame buffer: " << result << std::endl;
#endif
		throw std::exception("Could not create second frame buffer");
	}

}

VkSwapchainKHR* View::GetSwapchain() {
	return &this->swapchain;
}

VkExtent2D View::GetExtent2D() {
	return this->extent2D;
}

VkRenderPass* View::GetRenderPass() {
	return this->renderPasses;
}

VkFramebuffer* View::GetFrameBuffers(int* out_count) {
	if (out_count != nullptr) {
		*out_count = this->framebufferCount;
	}

	return this->framebuffers;
}

void View::SetClosingCallback(std::function<void()> new_callback) {
	this->closingCallback = new_callback;
}

void View::SetResizeCallback(std::function<void(int, int)> new_callback) {
	this->resizeCallback = new_callback;
}

void View::Resize(int x, int y) {

}