#version 330 core

in vec3 normal_ws;
in vec3 fragPos_ws;
in vec2 uv;

// Textures
uniform sampler2D dispMap;
uniform sampler2D normMap;
uniform sampler2D colorMap;
uniform sampler2D aoMap;

// Vectors
uniform vec3 cameraPos_ws;

uniform mat4 M;

// Scalars
uniform float k_diff, k_spec, specPow;

out vec4 fragmentColor;

vec3 lightPosition_ws = vec3(0.0, 0.0, 2.0);

void main() {

	// View direction
	vec3 viewDir_ws = normalize(fragPos_ws - cameraPos_ws);

	// Light direction
	vec3 lightDirection_ws = normalize(fragPos_ws - lightPosition_ws);

	// Phong shading

	// Diffuse light
	float diffuseLighting = max(0.0, dot(normal_ws, -lightDirection_ws));

	// Specular light
	vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 reflectionDir_ws = reflect(lightDirection_ws, normal_ws);
	float specularLight = pow(max(0.0, dot(reflectionDir_ws, -viewDir_ws)), specPow);

	float lightDist = length(lightPosition_ws - fragPos_ws);
	float atten = min(1.0, 10.0 / lightDist);
	
	// Composite lighting contributions
    fragmentColor = atten * k_diff * diffuseLighting * texture( colorMap, uv ) + k_spec * specularLight * specularColor;
}
