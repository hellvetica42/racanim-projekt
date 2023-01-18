#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

class Object {
public:
	Object(aiMesh* mesh, Material material, GLuint modelMatrixID);

	//void translate(glm::vec3 translation);
	void applyTransform(glm::mat4 transformMat);

	GLuint modelMatrixID;
	Mesh mesh;
	Transform transform;
};

