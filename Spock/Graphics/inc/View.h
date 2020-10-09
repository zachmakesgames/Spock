#pragma once
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "Device.h"



class View {

	public:
		View(Device* device);
		~View();


		
		
		VkFormat GetImageFormat();
		VkColorSpaceKHR GetColorSpaceFromFormat(VkFormat InFormat);
		VkPresentModeKHR GetPresentMode();

		VkBool32 IsFormatAvailable(VkFormat InFormat);

		VkRenderPass* GetRenderPass();

		VkSwapchainKHR GetSwapchain();

		VkFramebuffer* GetFrameBuffers();

	private:

		uint32_t SwapchainImageCount = 2;
		Device* ThisDevice = nullptr;

		uint32_t FormatCount;
		VkSurfaceFormatKHR* SupportedFormats = nullptr;

		uint32_t PresentModeCount;
		VkPresentModeKHR* PresentModes = nullptr;

		VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};

		VkSwapchainKHR Swapchain = {};

		uint32_t PresentImageCount = 0;
		VkImage* PresentImages = nullptr;

		VkImageView* PresentImageViews = nullptr;
		VkImageViewCreateInfo* PresentImageViewCreateInfo = nullptr;

		VkExtent3D Extent3D = {};

		VkImageCreateInfo ImageCreateInfo = {};

		VkImage* DepthStencilImage = nullptr;
		VkMemoryRequirements StencilRequirements;

		VkMemoryAllocateInfo MemoryAllocateInfo = {};
		VkDeviceMemory* StencilImageMemory = nullptr;

		VkImageViewCreateInfo ImageViewCreateInfo = {};
		VkImageView* ImageView = nullptr;

		VkRenderPassCreateInfo RenderPassCreateInfo = {};
		VkRenderPass* RenderPass = nullptr;

		VkImageView FramebufferAttachments[2];
		uint32_t FramebufferCount = 2;
		VkFramebuffer* Framebuffers = nullptr;
		VkFramebufferCreateInfo FramebufferCreateInfo = {};

	friend class Pipeline;
};