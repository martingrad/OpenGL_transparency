#version 330 core

// Input 
layout(location = 0) in vec3 vertexPos_ms;
layout(location = 1) in vec3 vertexNormal_ms;
layout(location = 2) in vec2 uvCoordinates;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPosition;

out vec3 normal_ws;
out vec3 fragPos_ws;
out vec2 uv;

void main()
{
	gl_Position = MVP * vec4(vertexPos_ms, 1.0);

	fragPos_ws = vec3(M * vec4(vertexPos_ms,1));
	uv = uvCoordinates;
	// TODO: Is this really in world space...?
	normal_ws = vec3(transpose(inverse(M)) * vec4(vertexNormal_ms, 1.0));
}
