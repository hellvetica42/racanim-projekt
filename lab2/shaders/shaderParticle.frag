#version 330 core

in vec4 Color;
in vec2 TexCoords;
out vec4 color;


uniform sampler2D sprite;

void main(){
	color = texture2D(sprite, TexCoords) * Color;
}