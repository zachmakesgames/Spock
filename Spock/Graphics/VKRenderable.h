#pragma once
#include "IRenderable.h"
#include "inc/Device.h"
#include "inc/Pipeline.h"
#include "inc/DescriptorSet.h"
#include "inc/Buffer.h"

#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/matrix_inverse.hpp"

class VKRenderable :
	public IRenderable
{
	protected:
		Device* device;
		Pipeline* pipeline;
		DescriptorSet* descriptorSet;
		Buffer* buffer;

		glm::vec3  eye;
		glm::vec3  look;
		glm::vec3  up;


		//Keep separate matrices will allow
		//individual transformations to be reset
		glm::mat4 Rotation;
		glm::mat4 Scale;
		glm::mat4 Translation;

		glm::mat4 uModelMatrix;
		glm::mat4 uNormalMatrix;
		glm::mat4 uViewMatrix;
		glm::mat4 uProjectionMatrix;

	public:
		inline VKRenderable(Pipeline* p, DescriptorSet* ds, Buffer* b) {
			this->pipeline = p;
			this->descriptorSet = ds;
			this->buffer = b;


			//Set matrices to identity
			this->uModelMatrix = glm::mat4(1.0f);
			this->uNormalMatrix = glm::mat4(1.0f);

			//Zero out Transformation matrices
			this->Rotation = glm::mat4(1.0f);
			this->Scale = glm::mat4(1.0f);
			this->Translation = glm::mat4(1.0f);

			this->uModelMatrix = this->Rotation * this->Scale * this->Translation;
			this->uNormalMatrix = glm::mat4(glm::inverseTranspose(glm::mat3(this->uModelMatrix)));
		
		
			//this is an absolutely horrible way to handle cameras and objects
			//but for a simple demo we can get away with it. This would need
			//to be changed drastically in a real application
			eye = glm::vec3(0, 0, 3);
			look = glm::vec3(0., 0., 0.);
			up = glm::vec3(0., 1., 0.);

			//set up camera stuff
			uViewMatrix = glm::lookAt(eye, look, up);
			uProjectionMatrix = glm::perspective(glm::radians(60.), (double)800 / (double)800, 0.1, 1000.);
			
			//this line fixes Vulkan's quirky +Y pointing down but may cause other
			//weird issues
			uProjectionMatrix[1][1] *= -1.;


			//give the object a color
			glm::vec3 uColor = glm::vec3(1, 0, 0);
			this->descriptorSet->SetUniform("colorBuf", &uColor, sizeof(glm::vec3), 0);

			//And dont forget lighting info
			glm::vec3 uLightPos = glm::vec3(3, 10, 5);
			this->descriptorSet->SetUniform("lightBuf", &uLightPos, sizeof(glm::vec3), 0);


			float uKa = 0.3;
			float uKd = 0.5;
			float uKs = 0.2;
			glm::vec4 uSpecularColor = glm::vec4(1, 1, 1, 1);
			float uShininess = 0.01;

			float lightVals[] = { uKa, uKd, uKs, uShininess };
			this->descriptorSet->SetUniform("ufLightBuf", &lightVals, sizeof(lightVals), 0);

			this->descriptorSet->SetUniform("ufLightBuf", &uSpecularColor, sizeof(uSpecularColor), sizeof(float) * 4);
		}

		inline Pipeline* GetPipeline() {
			return this->pipeline;
		}

		inline DescriptorSet* GetDescriptorSet() {
			return this->descriptorSet;
		}

		inline Buffer* GetBuffer() {
			return this->buffer;
		}

		inline void Update(float dt) {

		}

		inline void Init() {
			this->UpdateMatrices();
		}

		inline void UpdateMatrices() {
			this->uModelMatrix = this->Translation * this->Rotation * this->Scale;
			this->uNormalMatrix = glm::mat4(glm::inverseTranspose(glm::mat3(this->uModelMatrix)));

			glm::mat4 matrixSet[] = { uModelMatrix, uViewMatrix, uProjectionMatrix, uNormalMatrix };
			int sizem = sizeof(matrixSet);

			//Vertex shader uniforms
			//Then need to set the right buffer with the data
			this->descriptorSet->SetUniform("matBuf", matrixSet, sizeof(matrixSet), 0);
		}

		inline void SetWorldLocation(glm::vec3 NewLocation) {
			this->Translation = glm::translate(glm::mat4(1.0f), NewLocation); 
			this->UpdateMatrices();
		}

		inline void AddLocalTranslation(glm::vec3 LocalOffset) {
			this->Translation = glm::translate(this->Translation, LocalOffset); 
			this->UpdateMatrices();
		}

		inline void SetWorldRotation(glm::vec3 RotationAxis, float angle) {
			this->Rotation = glm::rotate(glm::mat4(1.0f), angle, RotationAxis); 
			this->UpdateMatrices();
		}

		inline void AddLocalRotation(glm::vec3 RotationAxis, float angle) {
			this->Rotation = glm::rotate(this->Rotation, angle, RotationAxis); 
			this->UpdateMatrices();
		}

		inline void SetWorldScale(glm::vec3 NewScale) {
			this->Scale = glm::scale(glm::mat4(1.0f), NewScale); 
			this->UpdateMatrices();
		}

		inline void AddLocalScale(glm::vec3 ScaleOffset) {
			this->Scale = glm::scale(this->Scale, ScaleOffset); 
			this->UpdateMatrices();
		}
};

