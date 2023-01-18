#include "Transform.h"

class Light : public Transform {
public:
	Light(glm::vec3 pos, glm::vec3 color);
	glm::vec3 color = glm::vec3(1.0,1.0,1.0);
};