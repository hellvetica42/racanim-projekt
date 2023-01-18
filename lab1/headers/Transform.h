#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           

class Transform {
	public:
		Transform() = default;
		Transform(glm::vec4 position);

		void setPosition(glm::vec4 position);
		void translate(glm::vec3 translation);

		void translateForward(float amount);
		void translateRight(float amount);
		void translateUp(float amount);

		void rotate(glm::vec3 axis, float angle);
		void setRotation(glm::vec4 targetForward);
		void rotateDirection(float xoffset, float yoffset);
		void rotateLocalX(float angle);
		void rotateLocalY(float angle);
		void rotateGlobalY(float angle);

		glm::vec4 getPosition();
		glm::mat4 getModelMatrix();
		glm::mat4 getViewMatrix();

	private:
		glm::vec4 position;
		glm::vec4 rotation;

		glm::vec4 directionForward;
		glm::vec4 directionRight;
		glm::vec4 directionUp;

		glm::mat4 rotationMat;

		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;

		float pitch, yaw;


};