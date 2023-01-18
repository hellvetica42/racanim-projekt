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
#include "Curve.h"
#include "CurveCreator.h"
#include "BCurve.h"
#include "BCurveCreator.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

int width =1000, height = 1000;
glm::vec2 cursorPosition(0, 0);
Camera camera = Camera(glm::vec3(20.0, 3.0, 17.0), width, height);
//Camera camera = Camera(glm::vec3(0.0, 0.0, -2.0), width, height);
glm::mat4 viewMat;
glm::vec4 eyePos;
std::vector<glm::vec4> controlPoints;
std::vector<glm::vec4> curvePoints;
Curve curve = Curve();
CurveCreator creator = CurveCreator();
//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* loadShader(char* path, char* naziv, bool loadGeom) {
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

	if (loadGeom) {
		return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
	}
	else {
		return new Shader(pathVert.c_str(), pathFrag.c_str());
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	float amount = 0.1;
	if (key == 87) {//W
		camera.translateForward(amount);
	}
	else if (key == 83) {//s
		camera.translateForward(-amount);
	}
	else if (key == 65) {//a
		camera.translateRight(-amount);
	}
	else if (key == 68) {//d
		camera.translateRight(amount);
	}
	else if (key == 81) {//q
		camera.translateUp(amount);
	}
	else if (key == 69) {//e
		camera.translateUp(-amount);
	}
	else if (key == 88) {//x
		//points.push_back(camera.getPosition());
		//std::cout << "Cam pos" << camera.getPosition().x << " " << camera.getPosition().y << " " << camera.getPosition().z << std::endl;
		//controlPoints.push_back(camera.getPosition());
		//if (controlPoints.size() > 3) {
		//	curvePoints = creator.getApproxCurve(controlPoints, 100);
		//	curve.setPoints(controlPoints, curvePoints);
		//}
		std::cout << glm::to_string(camera.getPosition()) << std::endl;
	}
	viewMat = camera.getViewMatrix();
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	float dx = (cursorPosition.x - xpos)/1;
	float dy = (cursorPosition.y - ypos)/1;
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
	//objPath.append("\\glava\\glava.obj"); //za linux pretvoriti u forwardslash
	objPath.append("\\aircraft747.obj"); //za linux pretvoriti u forwardslash
	std::cout << resPath << std::endl;

	std::string curvePath(resPath);
	curvePath.append("\\curvePoints.txt");


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

	glClearColor(0.25, 0.2, 0.2, 1); //boja brisanja platna izmedu iscrtavanja dva okvira

	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)

	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak X");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
	
	Shader *sjencar1 = loadShader(argv[0], "shader1",true);
	Shader *sjencar2 = loadShader(argv[0], "shader2",true);
	Shader *sjencar3 = loadShader(argv[0], "shader3",false);

	GLuint modelMatrixID1 = glGetUniformLocation(sjencar1->ID, "model");
	GLuint viewMatrixID1 = glGetUniformLocation(sjencar1->ID, "view");
	GLuint projectionMatrixID1 = glGetUniformLocation(sjencar1->ID, "projection");

	GLuint modelMatrixID2 = glGetUniformLocation(sjencar2->ID, "model");
	GLuint viewMatrixID2 = glGetUniformLocation(sjencar2->ID, "view");
	GLuint projectionMatrixID2 = glGetUniformLocation(sjencar2->ID, "projection");
	GLuint eyeID = glGetUniformLocation(sjencar2->ID, "eye");

	GLuint modelMatrixID3 = glGetUniformLocation(sjencar3->ID, "model");
	GLuint viewMatrixID3 = glGetUniformLocation(sjencar3->ID, "view");
	GLuint projectionMatrixID3 = glGetUniformLocation(sjencar3->ID, "projection");
	GLuint lineColor = glGetUniformLocation(sjencar3->ID, "lineColor");

	BCurve bCurve = BCurve::loadCurve(curvePath);
	glm::mat4 curveModelMatrix = bCurve.transform.getModelMatrix();

	Object object1 = Object(scene->mMeshes[0], modelMatrixID1);
	//object1.transform.translate(-bCurve.curveLines[0]);

	Object object2 = Object(scene->mMeshes[0], modelMatrixID2);
	object2.transform.translate(glm::vec3(-1,0,0));

	Renderer renderer = Renderer();
	renderer.addObject(&object1);
	renderer.addObject(&object2);


	glm::mat4 modelMat = object1.transform.getModelMatrix();
	glm::mat4 projectionMat = camera.getProjectionMatrix();
	viewMat = camera.getViewMatrix();


//	for (int i = 0; i < bCurve.curveLines.size(); i++)
//		std::cout << glm::to_string( bCurve.curveLines[i]) << std::endl;
	int segment = 0;
	float t = 0.0;
	float speed = 1;
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(false);
		eyePos = camera.getPosition();


		glViewport(0, 0, width, height);
		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/****************************/
		glUseProgram(sjencar1->ID);

		glm::vec4 curvePoint = bCurve.interpolate(segment, t);
		glm::vec4 targetRot = bCurve.interpolateRotation(segment, t);

		object1.transform.setPosition(curvePoint);
		object1.transform.setRotation(targetRot);

		modelMat = object1.transform.getModelMatrix();
		t += deltaTime * speed;
		if (t >= 1) {
			t = 0;
			segment += 1;
			//std::cout << segment << std::endl;
			if (segment >= bCurve.curveSegments.size()) {
				segment = 0;
			}
		}
		//std::cout << segment<<"   " << t << "     " <<bCurve.curveSegments.size() <<std::endl;

		glUniformMatrix4fv(modelMatrixID1, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(viewMatrixID1, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(projectionMatrixID1, 1, GL_FALSE, &projectionMat[0][0]);


		renderer.renderSingle(0);

		glUseProgram(sjencar2->ID);
		//glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(viewMatrixID2, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(projectionMatrixID2, 1, GL_FALSE, &projectionMat[0][0]);
		glUniform4f(eyeID, eyePos.x, eyePos.y, eyePos.z, eyePos.w);

		renderer.renderSingle(1);

		glm::vec3 lColor = glm::vec3(1.0, 0.0, 0.0);

		/*
		if (controlPoints.size() > 2) {
			glUseProgram(sjencar3->ID);
			glUniformMatrix4fv(modelMatrixID3, 1, GL_FALSE, &modelMat[0][0]);
			glUniformMatrix4fv(viewMatrixID3, 1, GL_FALSE, &viewMat[0][0]);
			glUniformMatrix4fv(projectionMatrixID3, 1, GL_FALSE, &projectionMat[0][0]);

			glUniform3fv(lineColor, 1, &lColor[0]);
			curve.drawCurve();

			lColor = glm::vec3(0.0, 1.0, 0.0);
			glUniform3fv(lineColor, 1, &lColor[0]);
			curve.drawControlLines();

		}
		*/

		glUseProgram(sjencar3->ID);
		glUniformMatrix4fv(modelMatrixID3, 1, GL_FALSE, &curveModelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixID3, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(projectionMatrixID3, 1, GL_FALSE, &projectionMat[0][0]);

		lColor = glm::vec3(0.0, 1.0, 0.0);
		glUniform3fv(lineColor, 1, &lColor[0]);

		bCurve.drawCurve();

		lColor = glm::vec3(1.0, 0.0, 0.0);
		glUniform3fv(lineColor, 1, &lColor[0]);
		bCurve.drawTangents();

		

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}   
		
	delete sjencar1;

	glfwTerminate();

    return EXIT_SUCCESS;
}
