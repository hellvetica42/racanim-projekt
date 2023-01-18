#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>

class Mesh {
	public:
		Mesh() = default;
		Mesh(aiMesh *mesh);

		aiMesh* mesh;
		void getBoundingBox(glm::vec3* mins, glm::vec3* maxes);
		void applyTransform(glm::mat4 transformMat);
		GLuint VAO;
		GLuint VBO[2];
		GLuint EBO;
		int numTriangles;
		std::vector<glm::vec4> vertices;

	private:
		unsigned int numVertices;
		unsigned _int16 numIndices;
		unsigned int *indices;
		std::vector<unsigned int> vIndices;


		void createVAOandEBO();

		void updateVAOandEBO();

};
