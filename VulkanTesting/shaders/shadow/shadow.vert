#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;


layout(binding = 1) uniform Light {
    vec4 position;
    vec4 objectColor;
    vec4 lightColor;
    vec4 viewPos;
    mat4 lightSpaceMatrix;
}lightBuffer;


layout(location = 0) in vec3 inPosition;


void main() {
    gl_Position = lightBuffer.lightSpaceMatrix * camera.model * vec4(inPosition, 1.0);
}

