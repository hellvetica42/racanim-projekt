#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#include <assimp/scene.h>           

#include <vector>
#include <iostream>

class Material {
public: 
	Material(aiMaterial* material);
	aiMaterial* material;
	aiColor3D ambientKai;
	aiColor3D diffuseKai;
	aiColor3D specularKai;

	glm::vec3 ambientK;
	glm::vec3 diffuseK;
	glm::vec3 specularK;
	float specularN;

};
