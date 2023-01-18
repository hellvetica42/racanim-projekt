#version 330 core
out vec4 FragColor;

in vec4 normal;
in vec4 vertex;
in vec2 texCoord;

uniform vec4 eye;
uniform vec3 colorAmbientK;
uniform vec3 colorDiffuseK;
uniform vec3 colorSpecularK;
uniform float specularN;
uniform vec4 lightPos;
uniform vec3 lightColor;

uniform sampler2D texData;

void main()
{
	vec4 toCamera = normalize(eye-vertex);
	vec4 toLight = normalize(lightPos - vertex);
	vec4 reflection = normalize(reflect(-toLight, normal));

	vec3 color = colorAmbientK * lightColor;
	color = color + texture(texData, texCoord).rgb * lightColor * max(dot(normal, toLight), 0);
    color = color + colorSpecularK * lightColor * pow(max(dot(toCamera, reflection), 0), specularN);

    FragColor = vec4(color, 1.0);  
} 
