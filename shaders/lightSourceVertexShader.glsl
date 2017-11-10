#version 330 core

/* Simple pass-through vertex shader */

// Input 
layout(location = 0) in vec3 vertexPos_ms;

uniform mat4 MVP;
uniform mat4 M;

void main()
{
	gl_Position = MVP * vec4(vertexPos_ms, 1.0);
}
