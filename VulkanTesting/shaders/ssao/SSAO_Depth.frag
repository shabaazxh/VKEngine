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
	bool isSSAOOn;
	float shadowScalar;
	float shadowContrast;
	float depthThreshold;
	int sampleAmount;
	int sampleTurns;
}kernelsamples;

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

	float radius = kernelsamples.radius;
	float bias = 0.025; //removing banding

	vec3 positions = texture(positionsTexture, vec2(1.0 - uvCoords.x, uvCoords.y)).xyz;
	vec3 normal = texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y)).rgb;
	vec3 randomVec = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz;

	vec3 viewSpacePositions = depthToPositions(vec2(1.0 - uvCoords.x, uvCoords.y));

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	vec3 plane = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz - vec3(1.0);

	float occlusion = 0.0;
	for(int i = 0; i < kernelsamples.sampleAmount; ++i) {
		//vec3 samplePos = reflect(kernelsamples.samples[i].xyz, plane);

		vec3 samplePos = TBN * kernelsamples.samples[i].xyz;// from tangent to view-space transform
		samplePos = viewSpacePositions + samplePos * radius; //add view-space kernel sample to view-space frag pos

		vec4 offset = vec4(samplePos, 1.0);
		offset = camera.proj * offset; //transform sample to clip space
		offset.xyz /= offset.w; // perspective divide
		offset.xy = offset.xy * 0.5 + 0.5; // transform to range 0-1

		//retrieve the z (depth) value of the offset position
		float sampleDepth = texture(positionsTexture, offset.xy).z;

		//float rangeCheck = smoothstep(0.0, 1.0, radius / abs(positions.z - sampleDepth));
		float rangeCheck = abs(viewSpacePositions.z - sampleDepth) < radius ? 1.0 : 0.0;
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);
	}

	occlusion = 1.0 - (occlusion / kernelsamples.sampleAmount);

	outColor = occlusion;
}


