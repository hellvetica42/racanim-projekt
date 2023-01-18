#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 eye;

in vec3 color[];

void main()
{

	vec3 normal = cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
						gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);

	vec3 center = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz)/3;

	if(dot(normal, eye.xyz-center) > 0){

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
	}



	EndPrimitive();
}