#version 450 


#define M_PI 3.1415926535897932384626433832795

layout(binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;

layout(binding = 4) uniform KernelSample {
	vec3 samples[64];
	mat4 projection;
	vec4 camera_eye;
	vec4 camera_direction;
	float z_far;
	mat4 mvMatrix;
}kernelsamples;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 3) in vec2 uv;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uvCoords;
layout(location = 3) out vec2 clip_XY;

void main()
{
	vec4 clip_pos = vec4(inPosition, 1.0);
	clip_XY = vec4(vec2(1.0 - uvCoords.x, uvCoords.y) * 2.0 - 1.0, 1.0, 1.0).xy;
	fragColor = inColor;
	uvCoords = uv.st;
	gl_Position = clip_pos;
}