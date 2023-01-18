#pragma once
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>           
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "BCurve.h"

class BCurveCreator {
public:
	BCurveCreator();
	static BCurve loadCurve(std::string fname);


private:
};
