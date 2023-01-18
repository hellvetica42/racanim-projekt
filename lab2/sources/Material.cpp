#pragma once
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/scene.h>           
#include<iostream>

#include "Material.h"

Material::Material(aiMaterial* material) {
	this->material = material;
	material->Get(AI_MATKEY_COLOR_AMBIENT, ambientKai);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseKai);
	material->Get(AI_MATKEY_COLOR_SPECULAR, specularKai);
	ambientK.r = ambientKai.r;
	ambientK.g = ambientKai.g;
	ambientK.b = ambientKai.b;

	diffuseK.r = diffuseKai.r;
	diffuseK.g = diffuseKai.g;
	diffuseK.b = diffuseKai.b;

	specularK.r = specularKai.r;
	specularK.g = specularKai.g;
	specularK.b = specularKai.b;

	material->Get(AI_MATKEY_SHININESS, specularN);
}
