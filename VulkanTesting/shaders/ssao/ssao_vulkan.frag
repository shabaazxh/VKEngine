#version 450

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;
layout(binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;

layout(binding = 4) uniform KernelSample {
	vec3 samples[64];
	mat4 projection;
}kernelsamples;

int kernelSize = 64;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out float fragColour;


const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

vec3 reconstructVSPosFromDepth(vec2 uv)
{
    float depth = texture(depthMap, uvCoords).r;
    float x = uv.x * 2.0f - 1.0f;
    float y = (1.0f - uv.y) * 2.0f - 1.0f;
    vec4 pos = vec4(x,y,depth, 1.0f);
    vec4 posVS = inverse(camera.proj) * pos;
    vec3 posNDC = posVS.xyz / posVS.w;
    return posNDC;
}

void main ()
{

    float radius = 0.5;
    float depth = texture(depthMap, uvCoords).r;


    if(depth == 0.0f)
    {
        fragColour = 1.0f;
        return;
    }

    vec3 normal = normalize(texture(gNormal, uvCoords).rgb * 2.0f - 1.0f);

    vec3 posVS = reconstructVSPosFromDepth(uvCoords);

    ivec2 depthTexSize = textureSize(depthMap, 0);
    ivec2 noiseTexSize = textureSize(texNoise, 0);
    float renderScale = 0.5;
    vec2 noiseUV = vec2(float(depthTexSize.x)/float(noiseTexSize.x), float(depthTexSize.y)/float(noiseTexSize.y)) * uvCoords * renderScale;

    vec3 randomVec = texture(texNoise, uvCoords * noiseScale).xyz;


    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(tangent, normal);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float bias = 0.01f;

    float occlusion = 0.0f;
    int sampleCount = 0;

    for(uint i = 0; i < 64; i++) {
        vec3 samplePos = TBN * kernelsamples.samples[i].xyz;
        samplePos = posVS + samplePos * radius;


        vec4 offset = vec4(samplePos, 1.0f);
        offset = camera.proj * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;
        offset.y = 1.0f - offset.y;

        vec3 reconstructedPos = reconstructVSPosFromDepth(offset.xy);
        vec3 sampledNormal = normalize(texture(gNormal, offset.xy).xyz * 2.0f - 1.0f);
        if (dot(sampledNormal, normal) > 0.99)
        {
            ++sampleCount;
        }

        else
        {
            float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(reconstructedPos.z - samplePos.z - bias));
            occlusion += (reconstructedPos.z <= samplePos.z - bias ? 1.0f : 0.0f) * rangeCheck;
            ++sampleCount;
        }
        
    }
    occlusion = 1.0 - (occlusion / float(max(sampleCount,1)));
    fragColour = occlusion;
}