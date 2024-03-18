#version 450 core

// vertex shader

// input: attribute named 'position with 3 float per vertex
layout(location 0) in vec3 position

void main(){
	gl_Position = vec4(position,1.0);
}