#pragma once
#include "global.h"
#include "c_device.h"
#include "c_instance.h"
#include "GLFW/glfw3.h"
#include <functional>
#include <iostream>


class c_view {
	public:
		c_view(c_instance *instance, c_device *device, int x, int y);
		VkSwapchainKHR *get_swapchain();
		VkExtent2D get_extent_2D();
		VkRenderPass *get_render_pass();
		VkFramebuffer *get_frame_buffers(int *out_count);
		void set_closing_callback(std::function<void()>);
		void set_resize_callback(std::function<void(int, int)>);
		void resize(int x, int y);

	private:
		c_instance *m_instance;
		c_device *m_device;
		VkExtent2D m_extent_2D;
		VkSurfaceKHR m_window_surface;
		GLFWwindow *m_window;
		VkSwapchainKHR m_swapchain;
		VkImage *m_present_images;
		VkImageView *m_present_image_views;
		VkExtent3D m_extent_3D;
		VkImage m_depth_stencil_image;
		VkRenderPass *m_render_passes;
		VkImageView m_framebuffer_attachments[2];
		VkFramebuffer *m_framebuffers;

		VkSwapchainCreateInfoKHR m_swapchain_create_info;
		VkImageViewCreateInfo *m_present_image_view_create_info;
		VkImageCreateInfo m_image_create_info;

		VkMemoryRequirements m_stencil_requirements;
		VkMemoryAllocateInfo m_stencil_memory_alloc_info;
		VkDeviceMemory *m_stencil_memory;


		VkImageViewCreateInfo m_image_view_create_info;
		VkImageView *m_image_view;


		VkRenderPassCreateInfo m_render_pass_create_info;

		VkFramebufferCreateInfo m_framebuffer_create_info;

		uint32_t m_present_image_count= 3;
		int m_render_pass_count= 1;
		int m_frame_view_buffer_attachment_count= 1;
		int m_framebuffer_count= 3;
		uint32_t m_swapchain_image_count= 3;

		std::function<void()> m_closing_callback;
		std::function<void(int, int)> m_resize_callback;
};