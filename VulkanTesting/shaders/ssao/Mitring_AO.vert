#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 3) in vec2 uv;


layout(binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uvCoords;
layout(location = 2) out vec3 worldCoordinatesCameraDirection;

void main()
{
	//gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
	gl_Position = vec4(inPosition, 1.0);
	fragColor = inColor;
	uvCoords = uv;
}