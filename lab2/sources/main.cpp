// Local Headers

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>

#include "Shader.h"
#include "FPSManager.h"
#include "Object.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "ParticleSystem.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

int width =1200, height = 1200;
glm::vec2 cursorPosition(0, 0);
Camera camera = Camera(glm::vec3(0.0, 5, 5.0), width, height);
ParticleSystem* ps1;
ParticleSystem* ps2;
glm::mat4 viewMat, inverseViewMat;
glm::vec4 eyePos;

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* loadShader(char* path, char* naziv, bool useGeom) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;
	std::string pathGeom;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	pathGeom.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
		pathGeom.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
		pathGeom.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");
	pathGeom.append(naziv);
	pathGeom.append(".geom");

	if (useGeom) {
		return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
	}
	else {
		return new Shader(pathVert.c_str(), pathFrag.c_str());
	}

}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	float amount = 0.01;
	if (key == 87) //W
		camera.translateForward(amount);
	if (key == 83) //s
		camera.translateForward(-amount);
	if (key == 65) //a
		camera.translateRight(-amount);
	if (key == 68) //d
		camera.translateRight(amount);
	if (key == 81) //q
		camera.translateUp(amount);
	if (key == 69) //e
		camera.translateUp(-amount);

	if (key == 70) //f
		ps1->position += glm::vec4(amount, 0, 0, 0);
	if (key == 71) //g
		ps1->position += glm::vec4(0, 0, amount, 0);
	if (key == 72) //h
		ps1->position += glm::vec4(-amount, 0, 0, 0);
	if (key == 84) //t
		ps1->position += glm::vec4(0, 0, -amount, 0);

	if (key == 74) //j
		ps2->startVelocity += glm::vec4(amount, 0, 0, 0);
	if (key == 75) //k
		ps2->startVelocity += glm::vec4(0, 0, amount, 0);
	if (key == 76) //l
		ps2->startVelocity += glm::vec4(-amount, 0, 0, 0);
	if (key == 73) //i
		ps2->startVelocity += glm::vec4(0, 0, -amount, 0);

	viewMat = camera.getViewMatrix();
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	float dx = (cursorPosition.x - xpos) / 5;
	float dy = (cursorPosition.y - ypos)/5;
	camera.rotateDirection(-dx, dy);
	cursorPosition = glm::vec2(xpos, ypos);
	viewMat = camera.getViewMatrix();
}

//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju poveyati pomocu glfwSetFramebufferSizeCallback
void framebuffer_size_callback(GLFWwindow * window, int Width, int Height)
{
	width = Width;
	height = Height;

	glViewport(0, 0, width, height);

}


int main(int argc, char * argv[]) {
	Assimp::Importer importer;

	std::string path(argv[0]);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath(resPath);
	objPath.append("\\glava\\glava.obj"); //za linux pretvoriti u forwardslash
	std::string particleTexPath(resPath);
	particleTexPath.append("\\cestica.bmp");

	const aiScene* scene = importer.ReadFile(objPath.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene) {
		std::cerr << importer.GetErrorString();
		return false;
	}


	std::cout << argv[0] << std::endl;
	GLFWwindow* window;

	glfwInit();
	gladLoadGL();

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);


	window = glfwCreateWindow(width, height, "Zadatak X", nullptr, nullptr);
	// provjeri je li se uspio napraviti prozor
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// dohvati sve dostupne OpenGL funkcije
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE); //ukljuci uklanjanje straznjih poligona -- za ovaj primjer je iskljuceno
	//glCullFace(GL_BACK); 

	glClearColor(0.15, 0.1, 0.1, 1); //boja brisanja platna izmedu iscrtavanja dva okvira

	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)

	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak X");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
	
	Shader *sjencar3 = loadShader(argv[0], "shaderPhong", false);

	GLuint modelMatrixID3 = glGetUniformLocation(sjencar3->ID, "model");
	GLuint viewMatrixID3 = glGetUniformLocation(sjencar3->ID, "view");
	GLuint projectionMatrixID3 = glGetUniformLocation(sjencar3->ID, "projection");
	GLuint eyeID3 = glGetUniformLocation(sjencar3->ID, "eye");
	GLuint lightPosID3 = glGetUniformLocation(sjencar3->ID, "lightPos");
	GLuint colorAmbientKID3 = glGetUniformLocation(sjencar3->ID, "colorAmbientK");
	GLuint colorDiffuseKID3 = glGetUniformLocation(sjencar3->ID, "colorDiffuseK");
	GLuint colorSpecularKID3 = glGetUniformLocation(sjencar3->ID, "colorSpecularK");
	GLuint specularNID3 = glGetUniformLocation(sjencar3->ID, "specularN");
	GLuint lightColorID3 = glGetUniformLocation(sjencar3->ID, "lightColor");

	Shader* particleShader = loadShader(argv[0], "shaderParticle", false);


	Material material = Material(scene->mMaterials[1]);
	Light light = Light(glm::vec3(3, 3, 3), glm::vec3(0.5, 0.5, 0.5));
	glm::vec4 lightPos = light.getPosition();

	Object object3 = Object(scene->mMeshes[0], material, modelMatrixID3);
	object3.transform.translate(glm::vec3(-4.5,0,0));

	Renderer renderer = Renderer();
	renderer.addObject(&object3);


	glm::mat4 projectionMat = camera.getProjectionMatrix();

	ps1 = new ParticleSystem(glm::vec4(0, 0, 0, 0), 50000, 0.001, glm::vec4(0,10,0,0),
										particleShader, "iskra2.png");

	ps2 = new ParticleSystem(glm::vec4(-10, 0, 0, 0), 5000, 0.01, glm::vec4(0,10,0,0),
										particleShader, "iskrica.tga");

	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(false);
		eyePos = camera.getPosition();
		viewMat = camera.getViewMatrix();
		inverseViewMat = glm::inverse(viewMat);
		//std::cout << glm::to_string(viewMat) << std::endl;


		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glViewport(0, 0, width, height);

		ps1->Update(deltaTime);
		ps1->renderParticles(projectionMat, viewMat, inverseViewMat, eyePos);

		ps2->Update(deltaTime);
		ps2->renderParticles(projectionMat, viewMat, inverseViewMat, eyePos);

		glUseProgram(sjencar3->ID);

		glUniformMatrix4fv(viewMatrixID3, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(projectionMatrixID3, 1, GL_FALSE, &projectionMat[0][0]);
		glUniform4f(eyeID3, eyePos.x, eyePos.y, eyePos.z, eyePos.w);
		glUniform4f(lightPosID3, lightPos.x,lightPos.y,lightPos.z, lightPos.w);
		glUniform3f(lightColorID3, light.color.r, light.color.g, light.color.b);
		glUniform3f(colorAmbientKID3, material.ambientK.r, material.ambientK.g, material.ambientK.b);
		glUniform3f(colorDiffuseKID3, material.diffuseK.r, material.diffuseK.g, material.diffuseK.b);
		glUniform3f(colorSpecularKID3, material.specularK.r, material.specularK.g, material.specularK.b);
		glUniform1f(specularNID3, material.specularN);

		renderer.renderSingle(0);
		

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}   
		
	delete sjencar3;
	delete ps1;

	glfwTerminate();

    return EXIT_SUCCESS;
}
