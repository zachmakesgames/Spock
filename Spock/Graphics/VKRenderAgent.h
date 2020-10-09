#pragma once
#include "IRenderAgent.h"
#include "VKRenderable.h"
#include "inc/Device.h"
#include "inc/Pipeline.h"
#include "inc/DescriptorSet.h"
#include "inc/Buffer.h"
#include "inc/View.h"
#include "inc/Instance.h"
#include "inc/Mesh.h"
#include "ShaderPack.h"

#include <string>

class VKRenderAgent :
	public IRenderAgent
{
	public:
		inline VKRenderAgent(std::string appName, int sizeX, int sizeY) {
			this->instance = new Instance(appName, sizeX, sizeY);
			this->device = new Device(this->instance);
			this->view = new View(this->device);


		}

		inline DescriptorSet* ProduceDescriptorSet(ShaderPack pack) {
			VkDescriptorPool dsPool;
			VkDescriptorPoolCreateInfo poolCreate = pack.GetDescriptorPoolCreateInfo();

			this->result = vkCreateDescriptorPool(this->device->GetLogicalDevice(), &poolCreate, nullptr, &dsPool);
			if (result != VK_SUCCESS) {
				std::cout << "Failed to create the descriptor pool\n";
			}

			DescriptorSet* returnSet = new DescriptorSet(this->device, &dsPool, &pack);

			return returnSet;
		}

		inline Pipeline* ProducePipeline(DescriptorSet* descriptorSet, ShaderPack* pack) {
			Pipeline* returnPipeline = new Pipeline(this->device, this->view, descriptorSet, descriptorSet->GetDescriptorSetCount(), pack);
			return returnPipeline;
		}

		inline Buffer* ProduceMeshBuffer(std::string filename, Mesh* m) {
			m = new Mesh(this->device);

			m->Load(filename);
			int rawSize;
			float* vertexData = m->GetRaw(rawSize);

			int vertexCount = rawSize * (3 + 3 + 2);

			Buffer* testbuffer = new Buffer(this->device, vertexCount, sizeof(float), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
			testbuffer->Fill(vertexData, vertexCount * sizeof(float));

			return testbuffer;
		}


		inline void Render(IRenderable* irenderable) {
			if (!renderStarted) {
				renderStarted = true;
				//do the initial drawing setup here
				// * ackquire next image in the chain
				// * begin command buffer
				// * begin render pass

				VkSemaphoreCreateInfo renderFinishedInfo;
				renderFinishedInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				renderFinishedInfo.pNext = nullptr;
				renderFinishedInfo.flags = 0;
				result = vkCreateSemaphore(this->device->GetLogicalDevice(), &renderFinishedInfo, nullptr, &(this->RenderFinishedSemaphore));


				VkSemaphoreCreateInfo			vsci;
				vsci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				vsci.pNext = nullptr;
				vsci.flags = 0;


				result = vkCreateSemaphore(this->device->GetLogicalDevice(), &vsci, nullptr, &(this->imageReadySemaphore));
				//uint32_t nextImageIndex;

				vkAcquireNextImageKHR(this->device->GetLogicalDevice(), this->view->GetSwapchain(), UINT64_MAX,
					this->imageReadySemaphore, VK_NULL_HANDLE, &(this->renderImageIndex));
				//REPORT( "vkCreateSemaphore" );



				VkCommandBufferBeginInfo		vcbbi;
				vcbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				vcbbi.pNext = nullptr;
				vcbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				//vcbbi.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;   <----- or could use this one??
				vcbbi.pInheritanceInfo = (VkCommandBufferInheritanceInfo*)nullptr;

				VkCommandBuffer cmdBuff = this->device->GetCommandBuffers()[this->renderImageIndex];

				result = vkBeginCommandBuffer(cmdBuff, &vcbbi);
				//REPORT( "vkBeginCommandBuffer" );

				VkClearColorValue			vccv;
				vccv.float32[0] = 0.1;
				vccv.float32[1] = 0.1;
				vccv.float32[2] = 0.1;
				vccv.float32[3] = 1.0;

				VkClearDepthStencilValue		vcdsv;
				vcdsv.depth = 1.f;
				vcdsv.stencil = 0;

				VkClearValue				vcv[2];
				vcv[0].color = vccv;
				vcv[1].depthStencil = vcdsv;

				VkOffset2D o2d = { 0, 0 };

				VkExtent2D e2d = { this->device->GetSurfaceExtent().width, this->device->GetSurfaceExtent().height };
				VkRect2D r2d = { o2d, e2d };

				VkRenderPassBeginInfo				vrpbi;
				vrpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				vrpbi.pNext = nullptr;
				vrpbi.renderPass = *this->view->GetRenderPass();
				vrpbi.framebuffer = this->view->GetFrameBuffers()[this->renderImageIndex];
				vrpbi.renderArea = r2d;
				vrpbi.clearValueCount = 2;
				vrpbi.pClearValues = vcv;		// used for VK_ATTACHMENT_LOAD_OP_CLEAR
				vkCmdBeginRenderPass(this->device->GetCommandBuffers()[this->renderImageIndex], &vrpbi, VK_SUBPASS_CONTENTS_INLINE);
			}
			


			VKRenderable* renderable = dynamic_cast<VKRenderable*>(irenderable);
			if (renderable == nullptr) {
				throw new std::exception("Invalid dynamic cast from type IRenderable to type VKRenderable\n");
				return;
			}


			
			

			vkCmdBindPipeline(this->device->GetCommandBuffers()[this->renderImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *(renderable->GetPipeline()->GetPipelineVK()));



			VkDescriptorSet* thisSet = renderable->GetDescriptorSet()->GetDescriptorSetVK();
			vkCmdBindDescriptorSets(this->device->GetCommandBuffers()[this->renderImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, renderable->GetPipeline()->GetPipelineLayout(), 0, renderable->GetDescriptorSet()->GetDescriptorSetCount(), thisSet, 0, (uint32_t*)nullptr);
			// dynamic offset count, dynamic offsets
		//vkCmdBindPushConstants( CommandBuffers[nextImageIndex], PipelineLayout, VK_SHADER_STAGE_ALL, offset, size, void *values );

		// all 3 buffer for geometry:



			VkBuffer buff;

			buff = (renderable->GetBuffer()->VkBuffer());

			VkDeviceSize offsets[1] = { 0 };


			vkCmdBindVertexBuffers(this->device->GetCommandBuffers()[this->renderImageIndex], 0, 1, &buff, offsets);               // 0, 1 = firstBinding, bindingCount



			//const uint32_t vertexCount = sizeof(VertexData)     / sizeof(VertexData[0]);
			//const uint32_t vertexCount = vertexCount;// sizeof(TestData) / sizeof(float) * 8;
			const uint32_t instanceCount = 1;
			const uint32_t firstVertex = 0;
			const uint32_t firstIndex = 0;
			const uint32_t firstInstance = 0;
			const uint32_t vertexOffset = 0;


			vkCmdDraw(this->device->GetCommandBuffers()[this->renderImageIndex], renderable->GetBuffer()->VertexCount(), instanceCount, firstVertex, firstInstance);




			

		}

		inline void Display() {
			if (renderStarted) {
				renderStarted = false;
				vkCmdEndRenderPass(this->device->GetCommandBuffers()[this->renderImageIndex]);

				vkEndCommandBuffer(this->device->GetCommandBuffers()[this->renderImageIndex]);

				VkFenceCreateInfo			vfci;
				vfci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				vfci.pNext = nullptr;
				vfci.flags = 0;

				VkFence renderFence;
				vkCreateFence(this->device->GetLogicalDevice(), &vfci, nullptr, &renderFence);

				VkPipelineStageFlags waitAtBottom = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

				VkQueue presentQueue;
				int graphicsQueFamily = 0;	//may need to actually find the right one here
				vkGetDeviceQueue(this->device->GetLogicalDevice(), graphicsQueFamily, 0, &presentQueue);
				// 0 = queueIndex
				VkSubmitInfo				vsi;
				vsi.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				vsi.pNext = nullptr;
				vsi.waitSemaphoreCount = 1;
				vsi.pWaitSemaphores = &(this->imageReadySemaphore);
				vsi.pWaitDstStageMask = &waitAtBottom;
				vsi.commandBufferCount = 1;
				vsi.pCommandBuffers = &this->device->GetCommandBuffers()[this->renderImageIndex];
				vsi.signalSemaphoreCount = 1;
				//vsi.signalSemaphoreCount = 0;
				vsi.pSignalSemaphores = &(this->RenderFinishedSemaphore);

				result = vkQueueSubmit(presentQueue, 1, &vsi, renderFence);	// 1 = submitCount


				result = vkWaitForFences(this->device->GetLogicalDevice(), 1, &renderFence, VK_TRUE, UINT64_MAX);	// waitAll, timeout


				vkDestroyFence(this->device->GetLogicalDevice(), renderFence, nullptr);

				VkPresentInfoKHR				vpi;
				vpi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				vpi.pNext = nullptr;
				vpi.waitSemaphoreCount = 0;
				vpi.pWaitSemaphores = (VkSemaphore*)nullptr;
				vpi.swapchainCount = 1;
				VkSwapchainKHR swapchain = this->view->GetSwapchain();
				vpi.pSwapchains = &swapchain;// SwapChain;
				vpi.pImageIndices = &(this->renderImageIndex);
				vpi.pResults = (VkResult*)nullptr;

				result = vkQueuePresentKHR(presentQueue, &vpi);

				vkDestroySemaphore(this->device->GetLogicalDevice(), this->imageReadySemaphore, nullptr);

				vkDestroySemaphore(this->device->GetLogicalDevice(), this->RenderFinishedSemaphore, nullptr);
			}
		}

		inline void Resize(int x, int y) {

		}

		
		inline bool WindowShouldClose() {
			return this->instance->ShouldClose();
		}

	private:
		Instance* instance = nullptr;
		Device* device = nullptr;
		View* view = nullptr;

		VkResult result;


		bool renderStarted = false;

		VkSemaphore imageReadySemaphore;
		VkSemaphore RenderFinishedSemaphore;

		uint32_t renderImageIndex = 0;
		


};

