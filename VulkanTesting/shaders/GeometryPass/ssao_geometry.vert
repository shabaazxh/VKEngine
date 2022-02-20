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
    bool invertedNormals;
}lightUniform;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 inTexCoord;


layout(location = 1) out vec3 FragPos;
layout(location = 4) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;


void main() {
    vec4 viewPos = ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    FragPos = viewPos.xyz;
    //FragPos = worldPos.xyz;

    TexCoords = inTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(ubo.view * ubo.model)));
    Normal = normalMatrix * (lightUniform.invertedNormals ? -aNormal : aNormal);

    gl_Position = ubo.proj * viewPos;

}