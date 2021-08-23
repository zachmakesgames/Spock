# Spock Class Responsibilities
This document describes which classes are responsible for different parts of the Vulkan pipeline. 

---

## Instance
* VkInstance

## Device
* Instance (reference)
* VkDevice
* VkPhysicalDevice
* VkQueue
* VkCommandPool (Graphics)
* VkCommandPool (Transfer)
* VkCommandBuffer (Graphics)
* VkCommandBuffer (Transfer)

## View
* Device (reference)
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

## Pipeline
* Device (reference)
* View (reference)
* VkPipelineLayout
* VkViewport
* VkRect2D (Scissor rect)
* VkPipeline

## DescriptorSet
* Device (reference)
* VkDescriptorSetLayout
* VkDescriptorSet
* VkDescriptorBufferInfo (Not needed?)
* VkWriteDescriptorSet (Not needed?)

## Buffer
* Device (reference)
* VkBuffer
* VkDeviceMemory
* VkDeviceSize

## VkRenderAgent
* Instance
* Device
* View
* Camera (active camera reference)

## VkRenderable
* Pipline (reference)
* DescriptorSet (reference)
* Buffer (reference)

## Camera
* DescriptorSet
* Buffer
* glm::mat4 (view matrix)
* glm::mat4 (projection matrix)
* float (near clip plain)
* float (far clip plain)
* float (aspect X)
* float (aspect Y)