#include "Transform.h"
#include "Light.h"

#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Light::Light(glm::vec3 pos, glm::vec3 color) : Transform(glm::vec4(pos, 1.0)) {
	this->color = color;
}
