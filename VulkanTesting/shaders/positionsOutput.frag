#version 450



layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;
layout(binding = 6) uniform sampler2D depthMap;

layout(location = 0) out vec4 outColor;

layout(binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;

void main() {

	float depth = texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y)).x;

	vec4 clipSpace = vec4(vec2(1.0 - uvCoords.x, uvCoords.y) * 2.0 - 1.0, depth, 1.0);
	vec4 viewSpace = inverse(camera.proj) * clipSpace;
	viewSpace.xyz /= viewSpace.w;

	outColor = vec4(vec3(viewSpace), 1.0);


}