# Spock Class Responsibilities
This document describes which classes are responsible for different parts of the Vulkan pipeline. 

---

## c_instance
* VkApplicationInfo
* VkInstanceCreateInfo
* VkInstance

## c_device
* VkPhysicalDevice
* VkDeviceQueueCreateInfo
* VkLayerProperties
* VkDeviceCreateInfo
* VkLogicalDevice
* VkQueue

## c_view
* VkSurfaceCapabilitiesKHR
* VkSurfaceKHR
* VkExtent2D
* VkSurfaceFormatKHR
* VkPresentModeKHR
* VkSwapchainCreateInfoKHR
* VkImage
* VkImageView
* VkImageViewCreateInfo


## c_buffer
* VkBufferCreateInfo
* VkBufferUsageFlags
* VkSharingMode
* VkDataBuffer
* VkMemoryAllocateInfo
* VkDeviceMemory

## c_image_sampler
* VkSamplerCreateInfo
* VkSampler

## c_image_buffer
* VkDeviceSize
* VkImage
* VkImageCreateInfo
* VkMemoryAllocateInfo
* VkDeviceMemory
* VkImageSubresource
* VkSubresourceLayout








---
Old stuff below, new stuff above
---

## c_instance
* VkInstance

## c_device
* c_instance (reference)
* VkDevice
* VkPhysicalDevice
* VkQueue
* VkCommandPool (Graphics)
* VkCommandPool (Transfer)
* VkCommandBuffer (Graphics)
* VkCommandBuffer (Transfer)

## c_view
* c_device (reference)
* VkExtent2D
* VkSurfaceKHR
* GLFWwindow
* VkSwapchainKHR
* VkImage (Present images)
* VkImageView (Present image view)
* VkExtent3D
* VkImage (Depth stencil image)
* VkRenderPass
* VkImageView (Framebuffer attachments)
* VkFrameBuffer* (Frame buffers)

## c_pipeline
* c_device (reference)
* c_view (reference)
* VkPipelineLayout
* VkViewport
* VkRect2D (Scissor rect)
* VkPipeline

## c_descriptor_set
* c_device (reference)
* VkDescriptorSetLayout
* VkDescriptorSet
* VkDescriptorBufferInfo (Not needed?)
* VkWriteDescriptorSet (Not needed?)

## c_buffer
* c_device (reference)
* VkBuffer
* VkDeviceMemory
* VkDeviceSize

## c_kk_render_agent
* c_instance
* c_device
* c_view
* c_camera (active camera reference)
* VkSemaphore (render finished)
* VkSemaphore (image ready)

## c_vk_renderable
* c_ipeline (reference)
* c_descriptor_set (reference)
* c_buffer (reference)

## c_camera
* c_descriptor_set
* c_buffer
* glm::mat4 (view matrix)
* glm::mat4 (projection matrix)
* float (near clip plain)
* float (far clip plain)
* float (aspect X)
* float (aspect Y)