#version 450

layout(binding = 1) uniform Light {
    vec4 position;
    vec4 objectColor;
    vec4 lightColor;
    vec4 viewPos;
    mat4 lightSpaceMatrix;
    float Linear;
    float Quadratic;
}LightUBO;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    vec3 albedo;
    vec4 lightPositions[2];
    vec4 lightColors[2];
    float metallic;
    float roughness;
    float ao;
    float time;
}camera;


layout(binding = 2) uniform sampler2D texSampler;
layout(binding = 3) uniform sampler2D DiffuseTexture;
layout(binding = 4) uniform sampler2D shadowMap;
layout(binding = 9) uniform sampler2D specularTexture;
layout(binding = 10) uniform sampler2D AOTexture;
layout(binding = 11) uniform sampler2D EmissionTexture;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec4 FragPos;
layout(location = 2) in vec3 Normal;
layout(location = 3) in float deltaTime;
layout(location = 4) in vec2 fragTexCoord;
layout(location = 5) in vec4 FragPosLightSpace;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num/denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1  * ggx2;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

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

// fragposLightSpace -> transformed verticies to light space 
float shadowResult(vec4 fragPosLightSpace)
{
    // convert light-space position into the range [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // convert the coordinates from [-1, 1] to [0,1] in order to correctly sample from depth map
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float bias = 0.005f;
    float shadow = currentDepth > closestDepth + bias ? 1.0 : 0.0;

    return shadow;
}

void main() {

/*     vec3 N = normalize(Normal);
    vec3 V = normalize(camera.cameraPosition - FragPos.xyz);

    vec3 Lo = vec3(0.0);

    // no need to loop -> if only have one light source
    for(int i = 0; i < 2; i++) {

        vec3 L = normalize(camera.lightPositions[i].xyz - FragPos.xyz);
        vec3 H = normalize(V + L);


        float distance = length(camera.lightPositions[i].xyz - FragPos.xyz);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = camera.lightColors[i].xyz * attenuation;

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, camera.albedo, camera.metallic);
        vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

        float NDF = DistributionGGX(N, H, camera.roughness);
        float G = GeometrySmith(N, V, L, camera.roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - camera.metallic;

        // calculate each lights outgoing reflectace value 
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * camera.albedo / PI + specular) * radiance * NdotL;
    }

    float shadow = shadowResult(FragPosLightSpace);
    vec3 ambient = vec3(0.03) * camera.albedo * camera.ao;
    vec3 color = ambient + Lo + (1.0 - shadow);

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); */




    float gamma = 2.2;

    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);

    //vec3 ambient = 0.3 * color;
    vec3 ambient = 0.3 * texture(DiffuseTexture, fragTexCoord).rgb * color;
    
    //Diffuse
    vec3 lightDir = normalize(LightUBO.position.xyz - FragPos.xyz);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * texture(DiffuseTexture, fragTexCoord).rgb * LightUBO.lightColor.xyz;

    // specular
    vec3 viewDir = normalize(LightUBO.viewPos.xyz - FragPos.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * texture(specularTexture, fragTexCoord).rgb * LightUBO.lightColor.xyz;

    //attenuation
    float max_distance = 1.5;
    float distance = length(LightUBO.position.xyz - FragPos.xyz);
    float attenuation = 1.0 / distance * distance;

    diffuse *= attenuation;
    specular *= attenuation;

    vec3 emission = texture(EmissionTexture, fragTexCoord).rgb;

    float shadow = shadowResult(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    lighting.rgb = pow(lighting.rgb, vec3(1.0/gamma));

    outColor = vec4(lighting, 1.0);
}