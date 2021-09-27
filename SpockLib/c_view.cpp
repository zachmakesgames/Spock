#include "c_view.h"

c_view::c_view(c_instance *instance, c_device *device, int x, int y) :
	m_instance(instance), m_device(device), m_closing_callback(nullptr), m_resize_callback(nullptr),
	m_extent_2D({})
{
	
	VkResult result;
	
	//Set up the window


	this->m_extent_2D.height= y;
	this->m_extent_2D.width= x;

	this->m_window= glfwCreateWindow(x, y, m_instance->get_instance_name().c_str(), NULL, NULL);
	result= glfwCreateWindowSurface(*this->m_instance->get_instance(), this->m_window, NULL, &this->m_window_surface);

	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error with glfwCreateWindowSurface: " << result << std::endl;
#endif
		throw std::exception("Could not create GLFW window surface");
	}



	//Check for surface support, vulkan will complain if we dont
	VkBool32 supported_surface= false;
	result= vkGetPhysicalDeviceSurfaceSupportKHR(*this->m_device->get_physical_device(), this->m_device->get_graphics_queue_family_index(), this->m_window_surface, &supported_surface);

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
	uint32_t format_count= -1;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*this->m_device->get_physical_device(), this->m_window_surface, &format_count, nullptr);
	VkSurfaceFormatKHR *supported_formats= new VkSurfaceFormatKHR[format_count];



	//Then populate the formats
	vkGetPhysicalDeviceSurfaceFormatsKHR(*this->m_device->get_physical_device(), this->m_window_surface, &format_count, supported_formats);
#ifdef DEBUG

	std::cout << "Found " << format_count << " supported formats" << std::endl;
	for (int i= 0; i < format_count; i++) {
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
	uint32_t present_mode_count= -1;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*this->m_device->get_physical_device(), this->m_window_surface, &present_mode_count, nullptr);
	VkPresentModeKHR *present_modes= new VkPresentModeKHR[present_mode_count];

	//Then populate the present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(*this->m_device->get_physical_device(), this->m_window_surface, &present_mode_count, present_modes);
#ifdef DEBUG
	std::cout << "Found " << present_mode_count << " presentation modes" << std::endl;
	for (int i= 0; i < present_mode_count; i++) {
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
	this->m_swapchain_create_info.sType= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	this->m_swapchain_create_info.pNext= nullptr;
	this->m_swapchain_create_info.flags= 0;
	this->m_swapchain_create_info.surface= this->m_window_surface;
	this->m_swapchain_create_info.minImageCount= this->m_swapchain_image_count;
	//VkFormat selectedFormat= this->GetImageFormat();
	this->m_swapchain_create_info.imageFormat= VK_FORMAT_B8G8R8A8_SRGB;
	this->m_swapchain_create_info.imageColorSpace= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;// this->GetColorSpaceFromFormat(selectedFormat);
	this->m_swapchain_create_info.presentMode= VK_PRESENT_MODE_IMMEDIATE_KHR;	///TODO: Add method to use different present modes
	this->m_swapchain_create_info.imageExtent= this->m_extent_2D;
	this->m_swapchain_create_info.imageArrayLayers= 1;
	this->m_swapchain_create_info.imageUsage= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	this->m_swapchain_create_info.preTransform= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	this->m_swapchain_create_info.compositeAlpha= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	this->m_swapchain_create_info.imageSharingMode= VK_SHARING_MODE_EXCLUSIVE;
	this->m_swapchain_create_info.queueFamilyIndexCount= this->m_device->get_graphics_queue_family_index();
	this->m_swapchain_create_info.pQueueFamilyIndices= nullptr;
	this->m_swapchain_create_info.oldSwapchain= VK_NULL_HANDLE;
	this->m_swapchain_create_info.clipped= VK_TRUE;



	result= vkCreateSwapchainKHR(*this->m_device->get_logical_device(), &this->m_swapchain_create_info, nullptr, &this->m_swapchain);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the swapchain: " << result << std::endl;
#endif
		throw std::exception("Could not create swapchain");
	}


	//Next get the swapchain images
	result= vkGetSwapchainImagesKHR(*this->m_device->get_logical_device(), this->m_swapchain, &this->m_swapchain_image_count, nullptr);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the present image count: " << result << std::endl;
#endif
		throw std::exception("Could not get swapchain images");
	}
	if (this->m_present_image_count < this->m_swapchain_image_count) {
#ifdef DEBUG
		std::cout << "Did not get the number of requested swap chain images!" << std::endl;
#endif
		throw std::exception("Swapchain/present image count mismatch");
	}


	this->m_present_images = new VkImage[this->m_present_image_count];
	result= vkGetSwapchainImagesKHR(*this->m_device->get_logical_device(), this->m_swapchain, &this->m_present_image_count, this->m_present_images);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error getting the present images: " << result << std::endl;
#endif
		throw std::exception("Error creating swapchain images");
	}


	//Set up Present Views
	this->m_present_image_views= new VkImageView[this->m_present_image_count];
	this->m_present_image_view_create_info= new VkImageViewCreateInfo[this->m_present_image_count];
	for (int i= 0; i < this->m_present_image_count; i++) {
		this->m_present_image_view_create_info[i].sType= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		this->m_present_image_view_create_info[i].pNext= nullptr;
		this->m_present_image_view_create_info[i].flags= 0;
		this->m_present_image_view_create_info[i].viewType= VK_IMAGE_VIEW_TYPE_2D;
		this->m_present_image_view_create_info[i].format= VK_FORMAT_B8G8R8A8_SRGB;	//need to correlate this to the available formats above?
		//this->m_present_image_view_create_info[i].format= this->GetImageFormat();	//There, now its linked to the available formats
		this->m_present_image_view_create_info[i].components.r= VK_COMPONENT_SWIZZLE_R;
		this->m_present_image_view_create_info[i].components.g= VK_COMPONENT_SWIZZLE_G;
		this->m_present_image_view_create_info[i].components.b= VK_COMPONENT_SWIZZLE_B;
		this->m_present_image_view_create_info[i].components.a= VK_COMPONENT_SWIZZLE_A;
		this->m_present_image_view_create_info[i].subresourceRange.aspectMask= VK_IMAGE_ASPECT_COLOR_BIT;
		this->m_present_image_view_create_info[i].subresourceRange.baseMipLevel= 0;
		this->m_present_image_view_create_info[i].subresourceRange.levelCount= 1;
		this->m_present_image_view_create_info[i].subresourceRange.baseArrayLayer= 0;
		this->m_present_image_view_create_info[i].subresourceRange.layerCount= 1;
		this->m_present_image_view_create_info[i].image= this->m_present_images[i];

		result= vkCreateImageView(*this->m_device->get_logical_device(), &this->m_present_image_view_create_info[i], nullptr, &this->m_present_image_views[i]);

		if (result != VK_SUCCESS) {
#ifdef DEBUG
			std::cout << "Vulkan encountered an error creating image view " << i << ": " << result << std::endl;
#endif
			throw std::exception("Could not create image view");
		}
	}





	//Set up the depth stencil
	this->m_extent_3D= { this->m_extent_2D.width, this->m_extent_2D.height, 1 };

	this->m_image_create_info.sType= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	this->m_image_create_info.pNext= nullptr;
	this->m_image_create_info.flags= 0;
	this->m_image_create_info.imageType= VK_IMAGE_TYPE_2D;
	this->m_image_create_info.format= VK_FORMAT_D32_SFLOAT_S8_UINT;
	this->m_image_create_info.extent= this->m_extent_3D;
	this->m_image_create_info.mipLevels= 1;
	this->m_image_create_info.arrayLayers= 1;
	this->m_image_create_info.samples= VK_SAMPLE_COUNT_1_BIT;
	this->m_image_create_info.tiling= VK_IMAGE_TILING_OPTIMAL;
	this->m_image_create_info.usage= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	this->m_image_create_info.sharingMode= VK_SHARING_MODE_EXCLUSIVE;
	this->m_image_create_info.queueFamilyIndexCount= 0;
	this->m_image_create_info.pQueueFamilyIndices= nullptr;
	this->m_image_create_info.initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;

	this->m_depth_stencil_image= {};
	result= vkCreateImage(*this->m_device->get_logical_device(), &this->m_image_create_info, nullptr, &this->m_depth_stencil_image);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the depth stencil: " << result << std::endl;
#endif
		throw std::exception("Could not create depth stencil image");
	}


	//Get information about what memory requirements the depth stencil needs
	vkGetImageMemoryRequirements(*this->m_device->get_logical_device(), this->m_depth_stencil_image, &this->m_stencil_requirements);

	VkPhysicalDeviceMemoryProperties memProperties = {};	///TODO: Do this in the Device class earlier and save for later use
	vkGetPhysicalDeviceMemoryProperties(*this->m_device->get_physical_device(), &memProperties);




	//Find memory that is device local
	uint32_t memoryIndex= -1;
	for (unsigned int i= 0; i < memProperties.memoryTypeCount; i++) {
		VkMemoryType memType= memProperties.memoryTypes[i];
		VkMemoryPropertyFlags memFlags= memType.propertyFlags;
		if ((this->m_stencil_requirements.memoryTypeBits & (1 << i)) != 0) {
			if ((memFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) {
				memoryIndex= i;
				break;
			}
		}
	}
	if (memoryIndex < 0) {
		throw std::exception("No device memory??!!");
	}

	//Set up the memory allocation info struct
	this->m_stencil_memory_alloc_info.sType= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	this->m_stencil_memory_alloc_info.pNext= nullptr;
	this->m_stencil_memory_alloc_info.allocationSize= this->m_stencil_requirements.size;
	this->m_stencil_memory_alloc_info.memoryTypeIndex= memoryIndex;

	//Then allocate the memory
	this->m_stencil_memory= new VkDeviceMemory();
	result= vkAllocateMemory(*this->m_device->get_logical_device(), &this->m_stencil_memory_alloc_info, nullptr, this->m_stencil_memory);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error allocating memory for the depth stencil: " << result << std::endl;
#endif
		throw std::exception("Could not allocate memory for depth stencil");
	}

	//And bind the memory
	result= vkBindImageMemory(*this->m_device->get_logical_device(), this->m_depth_stencil_image, *this->m_stencil_memory, 0);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error binding the depth stencil image memory: " << result << std::endl;
#endif
		throw std::exception("Could not bind stnecil image memory");
	}


	//
	//Then set up the image view
	this->m_image_view_create_info.sType= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	this->m_image_view_create_info.pNext= nullptr;
	this->m_image_view_create_info.flags= 0;
	this->m_image_view_create_info.image= this->m_depth_stencil_image;
	this->m_image_view_create_info.viewType= VK_IMAGE_VIEW_TYPE_2D;
	this->m_image_view_create_info.format= this->m_image_create_info.format;
	this->m_image_view_create_info.components.r= VK_COMPONENT_SWIZZLE_IDENTITY;
	this->m_image_view_create_info.components.g= VK_COMPONENT_SWIZZLE_IDENTITY;
	this->m_image_view_create_info.components.b= VK_COMPONENT_SWIZZLE_IDENTITY;
	this->m_image_view_create_info.components.a= VK_COMPONENT_SWIZZLE_IDENTITY;
	this->m_image_view_create_info.subresourceRange.aspectMask= VK_IMAGE_ASPECT_DEPTH_BIT;
	this->m_image_view_create_info.subresourceRange.baseMipLevel= 0;
	this->m_image_view_create_info.subresourceRange.levelCount= 1;
	this->m_image_view_create_info.subresourceRange.baseArrayLayer= 0;
	this->m_image_view_create_info.subresourceRange.layerCount= 1;

	this->m_image_view= new VkImageView();
	result= vkCreateImageView(*this->m_device->get_logical_device(), &this->m_image_view_create_info, nullptr, this->m_image_view);
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
	//vad[0].format= VK_FORMAT_B8G8R8A8_UNORM;
	vad[0].format= VK_FORMAT_B8G8R8A8_SRGB;
	//vad[0].format= this->GetImageFormat();
	vad[0].samples= VK_SAMPLE_COUNT_1_BIT;
	vad[0].loadOp= VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad[0].storeOp= VK_ATTACHMENT_STORE_OP_STORE;
	vad[0].stencilLoadOp= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad[0].stencilStoreOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad[0].initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;
	vad[0].finalLayout= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	vad[0].flags= 0;
	//vad[0].flags= VK_ATTACHMENT_DESCRIPTION_MAT_ALIAS_BIT;

	vad[1].format= VK_FORMAT_D32_SFLOAT_S8_UINT; //Note this is not using GetImageFormat because the depth format is different
	vad[1].samples= VK_SAMPLE_COUNT_1_BIT;
	vad[1].loadOp= VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad[1].storeOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad[1].stencilLoadOp= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad[1].stencilStoreOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad[1].initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;
	vad[1].finalLayout= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	vad[1].flags= 0;

	VkAttachmentReference				colorReference;
	colorReference.attachment= 0;
	colorReference.layout= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference				depthReference;
	depthReference.attachment= 1;
	depthReference.layout= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription				vsd;
	vsd.flags= 0;
	vsd.pipelineBindPoint= VK_PIPELINE_BIND_POINT_GRAPHICS;
	vsd.inputAttachmentCount= 0;
	vsd.pInputAttachments= (VkAttachmentReference*)nullptr;
	vsd.colorAttachmentCount= 1;
	vsd.pColorAttachments= &colorReference;
	vsd.pResolveAttachments= (VkAttachmentReference*)nullptr;
	vsd.pDepthStencilAttachment= &depthReference;
	vsd.preserveAttachmentCount= 0;
	vsd.pPreserveAttachments= (uint32_t*)nullptr;





	this->m_render_pass_create_info.sType= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	this->m_render_pass_create_info.pNext= nullptr;
	this->m_render_pass_create_info.flags= 0;
	this->m_render_pass_create_info.attachmentCount= 2;
	this->m_render_pass_create_info.pAttachments= vad;
	this->m_render_pass_create_info.subpassCount= 1;
	this->m_render_pass_create_info.pSubpasses= &vsd;
	this->m_render_pass_create_info.dependencyCount= 0;
	this->m_render_pass_create_info.pDependencies= nullptr;

	this->m_render_passes= new VkRenderPass();
	result= vkCreateRenderPass(*this->m_device->get_logical_device(), &this->m_render_pass_create_info, nullptr, this->m_render_passes);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the render pass: " << result << std::endl;
#endif
		throw std::exception("Could not create render passes");
	}


	//
	//Finally set up the frame buffer
	this->m_framebuffers= new VkFramebuffer[this->m_framebuffer_count];
	this->m_framebuffer_create_info.sType= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	this->m_framebuffer_create_info.pNext= nullptr;
	this->m_framebuffer_create_info.flags= 0;
	this->m_framebuffer_create_info.renderPass= *this->m_render_passes;
	this->m_framebuffer_create_info.attachmentCount= 2;
	this->m_framebuffer_create_info.pAttachments= this->m_framebuffer_attachments;
	this->m_framebuffer_create_info.width= this->m_extent_2D.width;
	this->m_framebuffer_create_info.height= this->m_extent_2D.height;
	this->m_framebuffer_create_info.layers= 1;

	this->m_framebuffer_attachments[0]= this->m_present_image_views[0];
	this->m_framebuffer_attachments[1]= *this->m_image_view;
	result= vkCreateFramebuffer(*this->m_device->get_logical_device(), &this->m_framebuffer_create_info, nullptr, &this->m_framebuffers[0]);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the first frame buffer: " << result << std::endl;
#endif
		throw std::exception("Could not create first frame buffer");
	}



	this->m_framebuffer_attachments[0]= this->m_present_image_views[1];
	this->m_framebuffer_attachments[1]= *this->m_image_view;
	result= vkCreateFramebuffer(*this->m_device->get_logical_device(), &this->m_framebuffer_create_info, nullptr, &this->m_framebuffers[1]);
	if (result != VK_SUCCESS) {
#ifdef DEBUG
		std::cout << "Vulkan encountered an error creating the second frame buffer: " << result << std::endl;
#endif
		throw std::exception("Could not create second frame buffer");
	}

}

VkSwapchainKHR *c_view::get_swapchain() {
	return &this->m_swapchain;
}

VkExtent2D c_view::get_extent_2D() {
	return this->m_extent_2D;
}

VkRenderPass *c_view::get_render_pass() {
	return this->m_render_passes;
}

VkFramebuffer *c_view::get_frame_buffers(int *out_count) {
	if (out_count != nullptr) {
		*out_count= this->m_framebuffer_count;
	}

	return this->m_framebuffers;
}

void c_view::set_closing_callback(std::function<void()> new_callback) {
	this->m_closing_callback= new_callback;
}

void c_view::set_resize_callback(std::function<void(int, int)> new_callback) {
	this->m_resize_callback= new_callback;
}

void c_view::resize(int x, int y) {
	///TODO: implement this
}