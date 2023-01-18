#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#include <assimp/scene.h>           

#include "BCurveCreator.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

BCurveCreator::BCurveCreator() {
}

BCurve BCurveCreator::loadCurve(std::string fname) {
	BCurve curve = BCurve();
	vector<float> row;
	std::string line, word;

	fstream file(fname, ios::in);
	if (file.is_open()) {
		while (getline(file, line)) {
			row.clear();
			stringstream str(line);
			while (getline(str, word, ','))
				row.push_back(stof(word));
			glm::vec4 point = glm::vec4(row[0], row[1], row[2], 1.0);
			curve.addPoint(point);
		}
	}
	else
		cout << "Could not open the file\n";
	
	return curve;
}

