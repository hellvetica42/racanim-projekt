#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>
#include <string>
#include "Shader.h"

struct Particle {
	glm::vec4 Position;
	glm::vec4 Velocity;
	glm::vec4 Color;
	float Life;
};

class ParticleSystem {
public:
	ParticleSystem(glm::vec4 position, unsigned int maxParticles, float rate,glm::vec4 startVelocity, 
		Shader * particleShader, std::string texturePath);
	Shader* particleShader;
	GLuint offsetID, colorID, projectionID, viewID, textureID, eyePosID, inverseViewID;
	glm::vec4 position, startVelocity;
	unsigned int maxParticles;
	float maxLife;
	float rate;
	unsigned int texture;
	std::string texturePath;

	GLuint VAO;
	GLuint billboard_vertex_buffer;
	GLuint texture_coordinates_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;

	std::vector<Particle> particles;

	unsigned int FirstUnusedParticle();
	void Update(float deltaTime);
	void RespawnParticle(Particle &particle, glm::vec4 offset);
	void renderParticles(glm::mat4 projection, glm::mat4 view, glm::mat4 inverseView, glm::vec4 eyePos);

private:
	int width, height, n_channels;
	unsigned char* textureData;
	float accumulator = 0.0f;

};