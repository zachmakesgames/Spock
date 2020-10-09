#include "../inc/View.h"
#define DEBUG


View::View(Device* device){

	VkResult result;

	this->ThisDevice = device;

	//Check which formats are available

	//First find out how many there are
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->ThisDevice->GetPhysicalDevice(), this->ThisDevice->GetSurface() , &this->FormatCount, nullptr);
	this->SupportedFormats = new VkSurfaceFormatKHR[this->FormatCount];

	//Then populate the formats
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->ThisDevice->GetPhysicalDevice(), this->ThisDevice->GetSurface(), &this->FormatCount, this->SupportedFormats);
#ifdef DEBUG
#ifdef VERBOSE
	std::cout << "Found " << this->FormatCount << " supported formats" << std::endl;
	for (int i = 0; i < this->FormatCount; ++i) {
		switch (this->SupportedFormats[i].format) {
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
		switch (this->SupportedFormats[i].colorSpace) {
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
#endif

	//Check which present modes are available
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->ThisDevice->GetPhysicalDevice(), this->ThisDevice->GetSurface(), &this->PresentModeCount, nullptr);
	this->PresentModes = new VkPresentModeKHR[this->PresentModeCount];

	//Then populate the present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->ThisDevice->GetPhysicalDevice(), this->ThisDevice->GetSurface(), &this->PresentModeCount, this->PresentModes);
#ifdef DEBUG
#ifdef VERBOSE
	std::cout << "Found " << this->PresentModeCount << " presentation modes" << std::endl;
	for (int i = 0; i < this->PresentModeCount; ++i) {
		switch (this->PresentModes[i]) {
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
#endif


	//Set up the swap chain creation info
	this->SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	this->SwapchainCreateInfo.pNext = nullptr;
	this->SwapchainCreateInfo.flags = 0;
	this->SwapchainCreateInfo.surface = this->ThisDevice->GetSurface();
	this->SwapchainCreateInfo.minImageCount = this->SwapchainImageCount;
	VkFormat selectedFormat = this->GetImageFormat();
	this->SwapchainCreateInfo.imageFormat = selectedFormat;
	this->SwapchainCreateInfo.imageColorSpace = this->GetColorSpaceFromFormat(selectedFormat);
	this->SwapchainCreateInfo.presentMode = this->GetPresentMode();
	this->SwapchainCreateInfo.imageExtent = this->ThisDevice->GetSurfaceExtent();
	this->SwapchainCreateInfo.imageArrayLayers = 1;
	this->SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	this->SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	this->SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	this->SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	this->SwapchainCreateInfo.queueFamilyIndexCount = 0;
	this->SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
	this->SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	this->SwapchainCreateInfo.clipped = VK_TRUE;

	result = vkCreateSwapchainKHR(this->ThisDevice->GetLogicalDevice(), &this->SwapchainCreateInfo, nullptr, &this->Swapchain);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the swapchain: " << result << std::endl;
#endif
		throw result;
	}


	//Next get the swapchain images
	result = vkGetSwapchainImagesKHR(this->ThisDevice->GetLogicalDevice(), this->Swapchain, &this->PresentImageCount, nullptr);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the present image count: " << result << std::endl;
#endif
		throw result;
	}
	if (this->PresentImageCount < this->SwapchainImageCount) {
#ifdef DEBUG
		std::cout << "Did not get the number of requested swap chain images!"<< std::endl;
#endif
		throw -1;
	}

	this->PresentImages = new VkImage[this->PresentImageCount];
	result = vkGetSwapchainImagesKHR(this->ThisDevice->GetLogicalDevice(), this->Swapchain, &this->PresentImageCount, this->PresentImages);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the present images: " << result << std::endl;
#endif
		throw result;
	}


	//Set up Present Views
	this->PresentImageViews = new VkImageView[this->PresentImageCount];
	this->PresentImageViewCreateInfo = new VkImageViewCreateInfo[this->PresentImageCount];
	for (int i = 0; i < this->PresentImageCount; ++i) {
		this->PresentImageViewCreateInfo[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		this->PresentImageViewCreateInfo[i].pNext = nullptr;
		this->PresentImageViewCreateInfo[i].flags = 0;
		this->PresentImageViewCreateInfo[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
		//this->PresentImageViewCreateInfo[i].format = VK_FORMAT_B8G8R8A8_SRGB;	//need to correlate this to the available formats above?
		this->PresentImageViewCreateInfo[i].format = this->GetImageFormat();	//There, now its linked to the available formats
		this->PresentImageViewCreateInfo[i].components.r = VK_COMPONENT_SWIZZLE_R;
		this->PresentImageViewCreateInfo[i].components.g = VK_COMPONENT_SWIZZLE_G;
		this->PresentImageViewCreateInfo[i].components.b = VK_COMPONENT_SWIZZLE_B;
		this->PresentImageViewCreateInfo[i].components.a = VK_COMPONENT_SWIZZLE_A;
		this->PresentImageViewCreateInfo[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		this->PresentImageViewCreateInfo[i].subresourceRange.baseMipLevel = 0;
		this->PresentImageViewCreateInfo[i].subresourceRange.levelCount = 1;
		this->PresentImageViewCreateInfo[i].subresourceRange.baseArrayLayer = 0;
		this->PresentImageViewCreateInfo[i].subresourceRange.layerCount = 1;
		this->PresentImageViewCreateInfo[i].image = this->PresentImages[i];

		result = vkCreateImageView(this->ThisDevice->GetLogicalDevice(), &this->PresentImageViewCreateInfo[i], nullptr, &this->PresentImageViews[i]);

		if (result != VK_SUCCESS) {
#ifdef DEBUG
			std::cout << "Vulkan encountered an error creating image view " << i << ": " << result << std::endl;
#endif
			throw result;
		}
	}


	//set up the depth stencil
	this->Extent3D = { this->ThisDevice->GetSurfaceExtent().width, this->ThisDevice->GetSurfaceExtent().height, 1 };

	this->ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	this->ImageCreateInfo.pNext = nullptr;
	this->ImageCreateInfo.flags = 0;
	this->ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	this->ImageCreateInfo.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	this->ImageCreateInfo.extent = this->Extent3D;
	this->ImageCreateInfo.mipLevels = 1;
	this->ImageCreateInfo.arrayLayers = 1;
	this->ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	this->ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	this->ImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	this->ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	this->ImageCreateInfo.queueFamilyIndexCount = 0;
	this->ImageCreateInfo.pQueueFamilyIndices = nullptr;
	this->ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	this->DepthStencilImage = new VkImage();
	result = vkCreateImage(this->ThisDevice->GetLogicalDevice(), &this->ImageCreateInfo, nullptr, this->DepthStencilImage);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the depth stencil: " << result << std::endl;
#endif
		throw result;
	}


	//Get information about what memory requirements the depth stencil needs
	vkGetImageMemoryRequirements(this->ThisDevice->GetLogicalDevice(), *this->DepthStencilImage, &this->StencilRequirements);

	VkPhysicalDeviceMemoryProperties memProperties = {};	///TODO: Do this in the Device class earlier and save for later use
	vkGetPhysicalDeviceMemoryProperties(this->ThisDevice->GetPhysicalDevice(), &memProperties);


	//Find memory that is device local
	uint32_t memoryIndex = -1;
	for (unsigned int i = 0; i < memProperties.memoryTypeCount; ++i) {
		VkMemoryType memType = memProperties.memoryTypes[i];
		VkMemoryPropertyFlags memFlags = memType.propertyFlags;
		if ((this->StencilRequirements.memoryTypeBits & (1 << i)) != 0) {
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
	this->MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	this->MemoryAllocateInfo.pNext = nullptr;
	this->MemoryAllocateInfo.allocationSize = this->StencilRequirements.size;
	this->MemoryAllocateInfo.memoryTypeIndex = memoryIndex;

	//Then allocate the memory
	this->StencilImageMemory = new VkDeviceMemory();
	result = vkAllocateMemory(this->ThisDevice->GetLogicalDevice(), &this->MemoryAllocateInfo, nullptr, this->StencilImageMemory);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating memory for the depth stencil: " << result << std::endl;
#endif
		throw result;
	}

	//And bind the memory
	result = vkBindImageMemory(this->ThisDevice->GetLogicalDevice(), *this->DepthStencilImage, *this->StencilImageMemory, 0);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error binding the depth stencil image memory: " << result << std::endl;
#endif
		throw result;
	}


	//
	//Then set up the image view
	this->ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	this->ImageViewCreateInfo.pNext = nullptr;
	this->ImageViewCreateInfo.flags = 0;
	this->ImageViewCreateInfo.image = *this->DepthStencilImage;
	this->ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	this->ImageViewCreateInfo.format = this->ImageCreateInfo.format;
	this->ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	this->ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	this->ImageViewCreateInfo.subresourceRange.levelCount = 1;
	this->ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	this->ImageViewCreateInfo.subresourceRange.layerCount = 1;

	this->ImageView = new VkImageView();
	result = vkCreateImageView(this->ThisDevice->GetLogicalDevice(), &this->ImageViewCreateInfo, nullptr, this->ImageView);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the image view: " << result << std::endl;
#endif
		throw result;
	}



	//
	//Then set up render passes
	//copied this from sample code developed by Dr. Mike Bailey at Oregon State University
	//Doesn't really need to be changed yet so I'm attributing the original author because reasons

	// need 2 - one for the color and one for the depth/stencil
	VkAttachmentDescription				vad[2];
	//vad[0].format = VK_FORMAT_B8G8R8A8_UNORM;
	//vad[0].format = VK_FORMAT_B8G8R8A8_SRGB;
	vad[0].format = this->GetImageFormat();
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

	this->RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	this->RenderPassCreateInfo.pNext = nullptr;
	this->RenderPassCreateInfo.flags = 0;
	this->RenderPassCreateInfo.attachmentCount = 2;
	this->RenderPassCreateInfo.pAttachments = vad;
	this->RenderPassCreateInfo.subpassCount = 1;
	this->RenderPassCreateInfo.pSubpasses = &vsd;
	this->RenderPassCreateInfo.dependencyCount = 0;
	this->RenderPassCreateInfo.pDependencies = nullptr;

	this->RenderPass = new VkRenderPass();
	result = vkCreateRenderPass(this->ThisDevice->GetLogicalDevice(), &this->RenderPassCreateInfo, nullptr, this->RenderPass);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the render pass: " << result << std::endl;
#endif
		throw result;
	}


	//
	//Finally set up the frame buffer
	this->Framebuffers = new VkFramebuffer[this->FramebufferCount];
	this->FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	this->FramebufferCreateInfo.pNext = nullptr;
	this->FramebufferCreateInfo.flags = 0;
	this->FramebufferCreateInfo.renderPass = *this->RenderPass;
	this->FramebufferCreateInfo.attachmentCount = 2;
	this->FramebufferCreateInfo.pAttachments = FramebufferAttachments;
	this->FramebufferCreateInfo.width = this->ThisDevice->GetSurfaceExtent().width;
	this->FramebufferCreateInfo.height = this->ThisDevice->GetSurfaceExtent().height;
	this->FramebufferCreateInfo.layers = 1;

	this->FramebufferAttachments[0] = this->PresentImageViews[0];
	this->FramebufferAttachments[1] = *this->ImageView;
	result = vkCreateFramebuffer(this->ThisDevice->GetLogicalDevice(), &this->FramebufferCreateInfo, nullptr, &this->Framebuffers[0]);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the first frame buffer: " << result << std::endl;
#endif
		throw result;
	}



	this->FramebufferAttachments[0] = this->PresentImageViews[1];
	this->FramebufferAttachments[1] = *this->ImageView;
	result = vkCreateFramebuffer(this->ThisDevice->GetLogicalDevice(), &this->FramebufferCreateInfo, nullptr, &this->Framebuffers[1]);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the second frame buffer: " << result << std::endl;
#endif
		throw result;
	}


}

View::~View() {
	if (this->SupportedFormats != nullptr) {
		delete[] this->SupportedFormats;
	}

	if (this->PresentModes != nullptr) {
		delete[] this->PresentModes;
	}
}

//A function to select the most fitting format
VkFormat View::GetImageFormat() {
	return this->SupportedFormats[0].format;
}

VkColorSpaceKHR View::GetColorSpaceFromFormat(VkFormat InFormat) {
	for (int i = 0; i < this->FormatCount; ++i) {
		if (this->SupportedFormats[0].format == InFormat) {
			return this->SupportedFormats[0].colorSpace;
		}
	}
}

VkPresentModeKHR View::GetPresentMode() {
	return {};
}

VkBool32 View::IsFormatAvailable(VkFormat InFormat) {
	for (int i = 0; i < this->FormatCount; ++i) {
		if (this->SupportedFormats[i].format == InFormat) {
			return true;
		}
	}
	return false;
}

VkRenderPass* View::GetRenderPass() {
	return this->RenderPass;
}

VkSwapchainKHR View::GetSwapchain() {
	return this->Swapchain;
}

VkFramebuffer* View::GetFrameBuffers() {
	return this->Framebuffers;
}