#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;

out vec3 color;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 eye;
uniform vec3 colorAmbientK;
uniform vec3 colorDiffuseK;
uniform vec3 colorSpecularK;
uniform float specularN;
uniform vec4 lightPos;
uniform vec3 lightColor;

void main()
{
	vec3 normal = cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
						gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);

	normal = normalize(normal);

	vec3 center = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz)/3;

		vec4 toLight = normalize(lightPos - vec4(center, 1));
		vec4 toCamera = normalize(eye - vec4(center, 1));
		vec4 reflection = normalize(reflect(-toLight, vec4(normal, 0)));

		color = colorAmbientK * lightColor;

		float diffuseCoef = max(dot(toLight, vec4(normal, 0)), 0.0);
		color = color + colorDiffuseK * lightColor * diffuseCoef;

		float specularCoef = pow(max(dot(toCamera, reflection), 0.0), specularN);
		//color = color + colorSpecularK * lightColor * specularCoef;
		color = color + vec3(1,0,0) * lightColor * specularCoef;

		vec4 tmp = projection * view  * gl_in[0].gl_Position;
		gl_Position = tmp / tmp.w;
		EmitVertex();
		tmp = projection * view  * gl_in[1].gl_Position;
		gl_Position = tmp / tmp.w;
		EmitVertex();
		tmp = projection * view  * gl_in[2].gl_Position;
		gl_Position = tmp / tmp.w;
		EmitVertex();
		tmp = projection * view  * gl_in[0].gl_Position;
		gl_Position = tmp / tmp.w;
		EmitVertex();



	EndPrimitive();
}