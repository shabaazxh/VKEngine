#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}ubo;

layout(binding = 1) uniform Light {
    vec4 position;
    vec4 objectColor;
    vec4 lightColor;
    vec4 viewPos;
    mat4 lightSpaceMatrix;
}lightUniform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec4 FragPos;
layout(location = 2) out vec3 Normal;
layout(location = 3) out float deltaTime;
layout(location = 4) out vec2 fragTexCoord;
layout(location = 5) out vec4 FragPosLightSpace;


void main() {

    FragPos = vec4(ubo.model * vec4(inPosition, 1.0));
    Normal = transpose(inverse(mat3(ubo.model))) * aNormal;
    fragTexCoord = inTexCoord;
    FragPosLightSpace = lightUniform.lightSpaceMatrix * vec4(FragPos.xyz, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    deltaTime = ubo.time;
    fragColor = inColor;

}

