#pragma once
#include "Device.h"
#include "Instance.h"
#include "GLFW/glfw3.h"
#include <functional>
#include <iostream>


class View {
	public:
		View(Instance* instance, Device* device, int x, int y);
		VkSwapchainKHR* GetSwapchain();
		VkExtent2D GetExtent2D();
		VkRenderPass* GetRenderPass();
		VkFramebuffer* GetFrameBuffers(int* out_count);
		void SetClosingCallback(std::function<void()>);
		void SetResizeCallback(std::function<void(int, int)>);
		void Resize(int x, int y);

	private:
		Instance* instance;
		Device* device;
		VkExtent2D extent2D;
		VkSurfaceKHR windowSurface;
		GLFWwindow* window;
		VkSwapchainKHR swapchain;
		VkImage* presentImages;
		VkImageView* presentImageViews;
		VkExtent3D extent3D;
		VkImage depthStencilImage;
		VkRenderPass* renderPasses;
		VkImageView framebufferAttachments[2];
		VkFramebuffer* framebuffers;

		VkSwapchainCreateInfoKHR swapchainCreateInfo;
		VkImageViewCreateInfo* presentImageViewCreateInfo;
		VkImageCreateInfo imageCreateInfo;

		VkMemoryRequirements stencilRequirements;
		VkMemoryAllocateInfo stencilMemoryAllocInfo;
		VkDeviceMemory* stencilMemory;


		VkImageViewCreateInfo imageViewCreateInfo;
		VkImageView* imageView;


		VkRenderPassCreateInfo renderPassCreateInfo;

		VkFramebufferCreateInfo framebufferCreateInfo;

		uint32_t presentImageCount = 3;
		int renderPassCount = 1;
		int frameViewBufferAttachmentCount = 1;
		int framebufferCount = 3;
		uint32_t swapchainImageCount = 3;

		std::function<void()> closingCallback;
		std::function<void(int, int)> resizeCallback;
};