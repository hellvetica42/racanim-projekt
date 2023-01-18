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

#include "Mesh.h"
#include "Transform.h"
#include "Object.h"

class Renderer {
public:
	Renderer();
	void addObject(Object * o);
	void renderAll();
	void renderSingle(int idx);

	std::vector<Object *> objects;
};
