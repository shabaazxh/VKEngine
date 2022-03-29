#version 450

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 7) uniform sampler2D positionsTexture;
layout(binding = 6) uniform sampler2D depthMap;

layout(std140, binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 cameraPosition;
    vec3 albedo;
    vec4 lightPositions[2];
    vec4 lightColors[2];
    float metallic;
    float roughness;
    float ao;
    float time;
}camera;


layout(std140, binding = 4) uniform KernelSample {
    mat4 projection;
    mat4 mvMatrix;
    vec4 samples[64];
    vec4 cameraEye;
    vec4 cameraCenter;
    float z_far;
    float radius;
    float bias;
    float scale;
    float sampleDirections;
    float num_sample_steps;
    float sampling_step;
    bool crytekSSAO;
    float shadowScalar;
    float shadowContrast;
    float depthThreshold;
    int sampleAmount;
    int sampleTurns;
    float ambientLightLevel;
    bool HBAO;
    bool AlchemyAO;
}ssaoparams;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out float outColor;

float zNear = 0.1;
float zFar = 1000;

int kernelSize = 64;
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

vec3 depthToPositions(vec2 tc)
{
	float depth = texture(depthMap, tc).x;

	vec4 clipSpace = vec4(tc * 2.0 - 1.0, depth, 1.0);
	vec4 viewSpace = inverse(camera.proj) * clipSpace;
	return viewSpace.xyz / viewSpace.w;
}

vec4 depthToNormals(vec2 tc)
{
	float depth = texture(depthMap, tc).x;

	vec4 clipSpace = vec4(tc * 2.0 - 1.0, depth, 1.0);
	vec4 viewSpace = inverse(camera.proj) * clipSpace;
	viewSpace.xyz /= viewSpace.w;

	vec3 pos = viewSpace.xyz;
	vec3 n = normalize(cross(dFdx(pos), dFdy(pos)));
	n *= - 1;

	return vec4(n, 1.0);
}


void main() {

	float radius = ssaoparams.radius;
	float bias = 0.025; //bias to remove banding

	vec3 viewSpacePositions = depthToPositions(vec2(1.0 - uvCoords.x, uvCoords.y));
	vec4 viewSpaceNormals = depthToNormals(vec2(1.0 - uvCoords.x, uvCoords.y));
	vec3 randomVec = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - viewSpaceNormals.xyz * dot(randomVec, viewSpaceNormals.xyz));
	vec3 bitangent = cross(viewSpaceNormals.xyz, tangent);
	mat3 TBN = mat3(tangent, bitangent, viewSpaceNormals.xyz);

	vec3 plane = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz - vec3(1.0);

	float occlusion = 0.0;
	for(int i = 0; i < ssaoparams.sampleAmount; ++i) {

		vec3 samplePos = reflect(TBN * ssaoparams.samples[i].xyz, plane);
		samplePos = viewSpacePositions + samplePos * radius; //add view-space kernel sample to view-space frag pos

		vec4 offset = vec4(samplePos, 1.0);
		offset = camera.proj * offset; //transform sample to clip space
		offset.xyz /= offset.w; // perspective divide
		offset.xy = offset.xy * 0.5 + 0.5; // transform to range 0-1

		//retrieve the z (depth) value of the offset position
		float sampleDepth = texture(positionsTexture, offset.xy).z;

		float rangeCheck = (viewSpacePositions.z - sampleDepth) < ssaoparams.radius? 1.0 : 0.0;
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / ssaoparams.sampleAmount);

	outColor = occlusion;
}


