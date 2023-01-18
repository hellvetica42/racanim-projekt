#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 norm;
layout (location = 2) in vec2 uvs;

uniform mat4 model;

uniform mat4 view;
uniform mat4 projection;

out vec4 normal;
out vec4 vertex;
out vec2 texCoord;

void main()
{
	vec4 tmp = projection*view*model*vec4(aPos.xyz, 1.0);
	normal = norm;
	vertex = tmp / tmp.w;
	texCoord = uvs;
    gl_Position = tmp / tmp.w;
}
