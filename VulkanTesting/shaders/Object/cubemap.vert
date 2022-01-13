#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}ubo;


layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 localPos;


void main() {
	localPos = inPosition;
	gl_Position = ubo.proj * ubo.view * vec4(localPos, 1.0);
}


