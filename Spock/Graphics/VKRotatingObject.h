#pragma once
#include "inc/Device.h"
#include "inc/Pipeline.h"
#include "inc/DescriptorSet.h"
#include "inc/Buffer.h"
#include "VKRenderable.h"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/matrix_inverse.hpp"

class VKRotatingObject :
	public VKRenderable
{
	public:   //temporary for testing
		glm::vec3 scale;
		glm::mat4 uScale;

		float rotationScale = 1.5f;

	private:

		glm::vec3  eye;
		glm::vec3  look;
		glm::vec3  up;

		glm::mat4 uRotate;
		glm::mat4 uModelMatrix;
		glm::mat4 uViewMatrix;
		glm::mat4 uProjectionMatrix;
		glm::mat4 uNormalMatrix;

		glm::vec3 uColor;
		glm::vec3 uLightPos;

		float uKa;
		float uKd;
		float uKs;
		glm::vec4 uSpecularColor;
		float uShininess;
		

	public:
		inline VKRotatingObject(Pipeline* p, DescriptorSet* ds, Buffer* b) : VKRenderable(p, ds, b) {}

	inline void Init() {
		int eyedist = 3;
		eye = glm::vec3(0, 0, eyedist);
		look = glm::vec3(0., 0., 0.);
		up = glm::vec3(0., 1., 0.);

		rotationScale = 1.0f;

		scale = glm::vec3(1.0f, 1.0f, 1.0f);

		uScale = glm::scale(glm::mat4(1.0f), scale);

		uRotate = glm::mat4(1.0f);

		uModelMatrix = glm::mat4(1.0f);		// identity

		uRotate = glm::rotate(uRotate, rotationScale, glm::vec3(0, 1, 0));

		//lets move the model around a little
		uModelMatrix = glm::translate(uModelMatrix, glm::vec3(0, -3, -10)) * uScale * uRotate;

		uViewMatrix = glm::lookAt(eye, look, up);
		uProjectionMatrix = glm::perspective(glm::radians(60.), (double)800 / (double)800, 0.1, 1000.);
		uProjectionMatrix[1][1] *= -1.;
		uNormalMatrix = glm::mat4(glm::inverseTranspose(glm::mat3(uModelMatrix)));


		glm::mat4 matrixSet[] = { uModelMatrix, uViewMatrix, uProjectionMatrix, uNormalMatrix };
		int sizem = sizeof(matrixSet);

		//Vertex shader uniforms
		//Then need to set the right buffer with the data
		this->descriptorSet->SetUniform("matBuf", matrixSet, sizeof(matrixSet), 0);
		
		uColor = glm::vec3(1, 0, 0);
		this->descriptorSet->SetUniform("colorBuf", &uColor, sizeof(glm::vec3), 0);


		//layout(std140, set = 2, binding = 0) uniform lightBuf{
		//vec3 uLightPos;
		//}Light;

		uLightPos = glm::vec3(3, 10, 5);
		this->descriptorSet->SetUniform("lightBuf", &uLightPos, sizeof(glm::vec3), 0);
		

		uKa = 0.3;
		uKd = 1;
		uKs = 0.3;
		uSpecularColor = glm::vec4(0, 0, 1, 1);
		uShininess = 0.01;

		float lightVals[] = { uKa, uKd, uKs, uShininess };
		this->descriptorSet->SetUniform("ufLightBuf", &lightVals, sizeof(lightVals), 0);

		this->descriptorSet->SetUniform("ufLightBuf", &uSpecularColor, sizeof(uSpecularColor), sizeof(float) * 4);
	}

	inline void Update(float dt) {
		uModelMatrix = glm::mat4(1.0f);		// identity

		//uModelMatrix = glm::rotate(uModelMatrix, (float)glm::radians(0.1* dt), glm::vec3(0, 0, 1));
		uRotate = glm::rotate(uRotate, rotationScale * dt, glm::vec3(0, 1, 0));


		uScale = glm::scale(glm::mat4(1.0f), scale);
		//lets move the model around a little
		uModelMatrix = glm::translate(uModelMatrix, glm::vec3(0, -3, -10)) * uScale * uRotate;

		uViewMatrix = glm::lookAt(eye, look, up);
		uProjectionMatrix = glm::perspective(glm::radians(60.), (double)800 / (double)800, 0.1, 1000.);
		uProjectionMatrix[1][1] *= -1.;
		uNormalMatrix = glm::mat4(glm::inverseTranspose(glm::mat3(uModelMatrix)));


		//matrixSet[] = { uModelMatrix, uViewMatrix, uProjectionMatrix, uNormalMatrix };
		//int sizem = sizeof(matrixSet);

		//Vertex shader uniforms
		//Then need to set the right buffer with the data
		glm::mat4 matrixSet[] = { uModelMatrix, uViewMatrix, uProjectionMatrix, uNormalMatrix };
		this->descriptorSet->SetUniform("matBuf", matrixSet, sizeof(matrixSet), 0);
	}

};

