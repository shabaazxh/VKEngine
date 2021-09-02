#version 450

layout(binding = 1) uniform Light {
    vec4 position;
    vec4 objectColor;
    vec4 lightColor;
    vec4 viewPos;
    mat4 lightSpaceMatrix;
}LightUBO;


layout(binding = 2) uniform sampler2D texSampler;
layout(binding = 3) uniform sampler2D DiffuseTexture;
layout(binding = 4) uniform sampler2D shadowMap;
layout(binding = 9) uniform sampler2D NormalTexture;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec4 FragPos;
layout(location = 2) in vec3 Normal;
layout(location = 3) in float deltaTime;
layout(location = 4) in vec2 fragTexCoord;
layout(location = 5) in vec4 FragPosLightSpace;

layout(location = 0) out vec4 outColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(LightUBO.position.xyz - FragPos.xyz);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {

    float gamma = 2.2;

    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);

    vec3 ambient = 0.3 * color;
    
    //Diffuse
    vec3 lightDir = normalize(LightUBO.position.xyz - FragPos.xyz);
    float diff = max(dot(lightDir, normal), 0.0);
    //vec3 diffuse = diff * LightUBO.lightColor.xyz;
    vec3 diffuse = diff * pow(texture(DiffuseTexture, fragTexCoord).rgb, vec3(gamma));

    // specular
    vec3 viewDir = normalize(LightUBO.viewPos.xyz - FragPos.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    //vec3 specular = spec * LightUBO.lightColor.xyz;
    vec3 specular = spec * pow(texture(NormalTexture, fragTexCoord).rgb, vec3(gamma));

    //attenuation
    float max_distance = 1.5;
    float distance = length(LightUBO.position.xyz - FragPos.xyz);
    float attenuation = 1.0 / distance * distance;

    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = ShadowCalculation(FragPosLightSpace);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

   // lighting.rgb = pow(lighting.rgb, vec3(1.0/gamma));

    outColor = vec4(lighting, 1.0);
}