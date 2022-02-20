#version 450

layout(binding = 3) uniform sampler2D DiffuseTexture;

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

// In
layout(location = 1) in vec3 FragPos;
layout(location = 4) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;

// Out
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gAlbedo;


void main() {
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gAlbedo.rgb = vec3(0.95);
    //gAlbedo.rgb = texture(DiffuseTexture, TexCoords).rgb;
}


