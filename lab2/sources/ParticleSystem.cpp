#pragma once

#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/scene.h>           
#include <assimp/scene.h>           
#include <string>

#include <vector>
#include <iostream>

#include <windows.h>
#include <string>

#include "ParticleSystem.h"
#include "Shader.h"

#include "stb_image.h"

using namespace std;

string getDir() {
	char buff[MAX_PATH];
	GetModuleFileName(NULL, buff, MAX_PATH);
	string::size_type position = string(buff).find_last_of("\\/");
	return string(buff).substr(0, position);
}

ParticleSystem::ParticleSystem(glm::vec4 position, unsigned int maxParticles, 
	float rate, glm::vec4 startVelocity, Shader* particleShader, std::string texturePath) {

	this->maxParticles = maxParticles;
	this->position = position;
	this->particleShader = particleShader;
	this->startVelocity = startVelocity;
	this->rate = rate;
	this->maxLife = 2.0f;
	this->texturePath = texturePath;

	this->offsetID = glGetUniformLocation(particleShader->ID, "offset");
	this->colorID= glGetUniformLocation(particleShader->ID, "color");
	this->projectionID = glGetUniformLocation(particleShader->ID, "projection");
	this->viewID = glGetUniformLocation(particleShader->ID, "view");
	this->eyePosID = glGetUniformLocation(particleShader->ID, "eyePos");
	this->inverseViewID = glGetUniformLocation(particleShader->ID, "inverseView");
	this->textureID = glGetUniformLocation(particleShader->ID, "sprite");

	string dirPath(getDir());
	string resPath(dirPath);
	resPath.append("\\resources\\");
	resPath.append(texturePath);
	textureData = stbi_load(resPath.c_str(), &width, &height, &n_channels, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textureData && n_channels == 4) {
		std::cout << "4 channel texture" << std::endl;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	}
	else if (textureData && n_channels == 3) {
		std::cout << "3 channel texture" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	}
	else {
		std::cout << "Failed to laod texture" << std::endl;
	}


	const GLfloat billboard_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
	};

	const GLfloat texture_buffer_data[] = {
		1.0, 1.0,
		0.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
	};


	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_vertex_buffer_data),
		billboard_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &texture_coordinates_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_buffer_data),
		texture_buffer_data, GL_STATIC_DRAW);


	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);


	for (int i = 0; i < maxParticles; i++) {
		Particle pt;
		pt.Position = position;
		pt.Velocity = startVelocity;
		pt.Color = glm::vec4(1, 0, 0, 1);
		pt.Life = 0.0;
		particles.push_back(pt);
	}

}

void ParticleSystem::Update(float deltaTime) {
	accumulator += deltaTime;

	unsigned int n_new_particles = (int)(accumulator / rate);
	if (n_new_particles > 1000000)
		n_new_particles = 0;
	if (n_new_particles > 0)
		accumulator = 0.0;

	glm::vec4 offset = glm::vec4(0.0f);

	for (unsigned int i = 0; i < n_new_particles; i++) {
		unsigned int unusedParticle = FirstUnusedParticle();
		RespawnParticle(particles[unusedParticle], offset);
	}

	for (unsigned int i = 0; i < maxParticles; i++) {
		Particle& p = particles[i];
		if (p.Life > 0.0f) {
			p.Life -= deltaTime;
			p.Velocity -= glm::vec4(0,9.81,0,0) * deltaTime;
			p.Position += p.Velocity * deltaTime;
			p.Color = glm::vec4(1, 0, 0, 1) + glm::vec4(0, 1, 1, 0) * (1 - p.Life/maxLife);
		}
	}

}

unsigned int lastUsedParticle = 0;
unsigned int ParticleSystem::FirstUnusedParticle() {

	for (unsigned int i = lastUsedParticle; i < this->maxParticles; i++) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (unsigned int i = 0; i < lastUsedParticle; i++) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	
	lastUsedParticle = 0;
	return 0;
}

void ParticleSystem::RespawnParticle(Particle& particle, glm::vec4 offset) {
	float randomX = ((rand() % 100) - 50) / 100.0f;
	float randomY = ((rand() % 100) - 50) / 100.0f;
	float rColor = ((rand() % 100) / 100.0f);
	float gColor = ((rand() % 100) / 100.0f);
	float bColor = ((rand() % 100) / 100.0f);
	particle.Position = this->position + glm::vec4(randomX, 0, randomY, 0);
	particle.Color = glm::vec4(rColor, 0, 0, 1.0f);
	particle.Velocity = startVelocity + glm::vec4(randomX*3, 0, randomY*3, 0);
	particle.Life = maxLife;
	//std::cout << "Respawned particle" << endl;
	//std::cout << particle.Color.r << endl;
}

void ParticleSystem::renderParticles(glm::mat4 projection, glm::mat4 view, glm::mat4 inverseView, glm::vec4 eyePos) {
	glUseProgram(this->particleShader->ID);
	glEnableVertexAttribArray(0);

	vector<glm::vec4> particle_positions;
	vector<glm::vec4> particle_colors;
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].Life > 0.0f) {
			particle_positions.push_back(particles[i].Position);
			particle_colors.push_back(particles[i].Color);
		}
	}


	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniform4f(eyePosID, eyePos.x, eyePos.y, eyePos.z, eyePos.w);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(inverseViewID, 1, GL_FALSE, &inverseView[0][0]);
	//glUniform1i(textureID, 3);

	if(particle_positions.size() == 0){
		//std::cout << "No living particles" << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particle_positions.size() * sizeof(GLfloat) * 4, &particle_positions[0].x);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particle_colors.size() * sizeof(GLfloat) * 4, &particle_colors[0].r);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_buffer);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 0);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particle_positions.size());
	glBindVertexArray(0);

}