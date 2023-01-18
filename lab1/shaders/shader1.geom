#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices=4) out;

void main()
{
	vec3 vert0 = vec3(gl_in[0].gl_Position.xy, 1.0);
	vec3 vert1 = vec3(gl_in[1].gl_Position.xy, 1.0);
	vec3 vert2 = vec3(gl_in[2].gl_Position.xy, 1.0);

	//vec3 vert0 = vec3(gl_in[0].gl_Position.xy, 1.0)/gl_in[0].gl_Position.w;
	//vec3 vert1 = vec3(gl_in[1].gl_Position.xy, 1.0)/gl_in[1].gl_Position.w;
	//vec3 vert2 = vec3(gl_in[2].gl_Position.xy, 1.0)/gl_in[2].gl_Position.w;

	vec3 edge0 = cross(vert0, vert1);
	vec3 edge1 = cross(vert1, vert2);
	vec3 edge2 = cross(vert2, vert0);

	if(dot(edge0, vert2) > 0 && dot(edge1, vert0) > 0 && dot(edge2, vert1) > 0){
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}