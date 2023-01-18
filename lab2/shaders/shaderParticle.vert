#version 330 core

layout (location = 0) in vec3 billboard_v;
layout (location = 1) in vec4 position; //vec4 position
layout (location = 2) in vec4 colors;
layout (location = 3) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 inverseView;
uniform vec4 eyePos;

out vec4 Color;
out vec2 TexCoords;

void main(){
	Color = colors;
	TexCoords = texCoords;

	vec3 dir = normalize(position - eyePos).xyz;
	float scale = 0.4;

	mat3 rotMat = mat3(
    vec3(dir.z, 0.0, -dir.x),
    vec3(0.0, 1.0, 0.0),
    vec3(dir.x, 0.0, dir.z)
	  );

	vec4 vertexWorldSpace = vec4(rotMat * billboard_v * scale, 1) + position;
	vertexWorldSpace = vertexWorldSpace/vertexWorldSpace.w;


	gl_Position = projection * view * vertexWorldSpace;
}