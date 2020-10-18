#pragma once
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/matrix_inverse.hpp"


struct CameraMatrixSet {
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
};

class Camera {
	private:
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;

		glm::vec3  eye;
		glm::vec3  look;
		glm::vec3  up;

		double FieldOfView;
		double AspectRatio;
		double NearClipping;
		double FarClipping;

		double Top;
		double Left;
		double Bottom;
		double Right;

		bool UsePerspective;

	public:
		inline Camera() {
			this->eye = glm::vec3(0, 0, 3);
			this->look = glm::vec3(0., 0., 0.);
			this->up = glm::vec3(0., 1., 0.);

			this->ProjectionMatrix = glm::mat4(1.0f);
			this->ViewMatrix = glm::mat4(1.0f);
		}

		inline virtual glm::mat4 GetViewMatrix() {
			return glm::lookAt(this->eye, this->look, this->up);
		}

		inline virtual glm::mat4 GetPerspectiveProjectionMatrix(double fovy, double aspect, double near, double far) {
			//glm::mat4 projection = glm::perspective(glm::radians(60.), (double)800 / (double)800, 0.1, 1000.);
			glm::mat4 projection = glm::perspective(fovy, aspect, near, far);
			projection[1][1] *= -1;
			return projection;
		}

		inline void SetPosition(glm::vec3 newPosition) {
			this->eye = newPosition;
		}

		inline void SetFieldOfView(double NewFOV) {
			this->FieldOfView = NewFOV;
			this->_CalculateProjection();
		}

		inline void SetAspectRatio(double NewAspect) {
			this->AspectRatio = NewAspect;
			this->_CalculateProjection();
		}

		inline struct CameraMatrixSet GetMatrixSet() {
			CameraMatrixSet Set;
			Set.ProjectionMatrix = this->ProjectionMatrix;
			Set.ViewMatrix = this->ViewMatrix;
			return Set;
		}

	protected:
		inline void _CalculateLookAt() {
			this->ViewMatrix = glm::lookAt(this->eye, this->look, this->up);
		}

		inline void _CalculateProjection() {
			if (this->UsePerspective) {
				this->ProjectionMatrix = glm::perspective(this->FieldOfView, this->AspectRatio, this->NearClipping, this->FarClipping);
			}
			else {
				this->ProjectionMatrix = glm::ortho(this->Left, this->Right, this->Bottom, this->Top);
			}
		}
};