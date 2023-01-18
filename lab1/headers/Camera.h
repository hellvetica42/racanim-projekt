#include "Transform.h"

class Camera : public Transform {
public:
	Camera(glm::vec3 pos, int width, int height);
	glm::mat4 getProjectionMatrix();

	int width, height;
};