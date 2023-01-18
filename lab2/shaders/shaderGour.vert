#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 norm;

uniform mat4 model;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 eye;
uniform vec3 colorAmbientK;
uniform vec3 colorDiffuseK;
uniform vec3 colorSpecularK;
uniform float specularN;
uniform vec4 lightPos;
uniform vec3 lightColor;

out vec3 color;

void main()
{
/*
    vec4 normal = vec4(normalize(norm), 0);
    vec4 toCamera = normalize(eye - vec4(aPos, 1));
    vec4 toLight = normalize(lightPos - vec4(aPos, 1));
	vec4 reflection = normalize(reflect(-toLight, normal));
    color = colorAmbientK * lightColor;
    color = color + colorDiffuseK * lightColor * max(dot(toLight, normal), 0.0); 
    //color = color + colorSpecularK * lightColor * pow(max(dot(toCamera, reflection), 0), 10);
    color = color + vec3(1,0,0) * lightColor * pow(max(dot(toCamera, reflection), 0), 10);
*/

    vec4 vertex = model*vec4(aPos.xyz, 1.0);
	vec4 normal = normalize(model*norm);
	vec4 toCamera = normalize(eye-vertex);
	vec4 toLight = normalize(lightPos - vertex);
	vec4 reflection = normalize(reflect(-toLight, normal));

	color = colorAmbientK * lightColor;
	//color = color + vec3(1,0,0)* lightColor * max(dot(normal, toLight), 0);
	color = color + colorDiffuseK * lightColor * max(dot(normal, toLight), 0);
    color = color + vec3(1,0,0) * lightColor * pow(max(dot(toCamera, reflection), 0), specularN);


   vec4 tmp = projection*view*model*vec4(aPos.xyz, 1.0);
    gl_Position = tmp / tmp.w;
    //gl_Position = projection*view*model*aPos;
}
