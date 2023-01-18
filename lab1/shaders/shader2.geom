#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices=4) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 eye;

void main()
{

	vec3 normal = cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
						gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);

	vec3 center = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz)/3;

	if(dot(normal, eye.xyz-center) > 0){

		gl_Position = projection * view * model * gl_in[0].gl_Position;
		EmitVertex();
		gl_Position = projection * view * model * gl_in[1].gl_Position;
		EmitVertex();
		gl_Position = projection * view * model * gl_in[2].gl_Position;
		EmitVertex();
		gl_Position = projection * view * model * gl_in[0].gl_Position;
		EmitVertex();
	}



	EndPrimitive();
}