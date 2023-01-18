#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>

#include "Material.h"

class Mesh {
	public:
		Mesh() = default;
		Mesh(aiMesh *mesh, Material material);

		aiMesh* mesh;
		void getBoundingBox(glm::vec3* mins, glm::vec3* maxes);
		void applyTransform(glm::mat4 transformMat);
		GLuint VAO;
		GLuint VBO[3];
		GLuint EBO;
		unsigned int texture;
		int numTriangles;
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> normals;
		std::vector<glm::vec2> uvs;

	private:
		unsigned int numVertices;
		unsigned _int16 numIndices;
		unsigned int *indices;
		std::vector<unsigned int> vIndices;
		int width, height, nrChannels;
		unsigned char* textureData;


		void createVAOandEBO();

		void updateVAOandEBO();

};
