#version 450

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D gAlbedo;
layout(binding = 4) uniform sampler2D ssao;

layout(binding = 5) uniform Light {
    vec4 position;
    vec4 objectColor;
    vec4 lightColor;
    vec4 viewPos;
    mat4 lightSpaceMatrix;
    float Linear;
    float Quadratic;
}LightUBO;


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out float outColor;

void main() {

    vec2 texelSize = 1.0 / vec2(textureSize(ssao, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for(int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssao, vec2(1.0 - uvCoords.x, uvCoords.y) + offset).r;
        }
    }

    outColor = result / (4.0 * 4.0);
    
}