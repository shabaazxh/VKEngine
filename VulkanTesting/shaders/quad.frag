#version 450

layout(binding = 1) uniform sampler2D sceneImage;
layout(binding = 2) uniform sampler2D SSAOImage;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 color = texture(sceneImage, vec2(1.0 - uvCoords.x, uvCoords.y)).rgb;
	
	float ssao = texture(SSAOImage, vec2(1.0 - uvCoords.x, uvCoords.y)).r;

	color *= ssao;

	//outColor = vec4(vec3(texture(SSAOImage, vec2(1.0 - uvCoords.x, uvCoords.y))), 1.0);
	outColor = vec4(color, 1.0);
}