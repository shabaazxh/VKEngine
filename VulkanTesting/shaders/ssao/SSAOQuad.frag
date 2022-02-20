#version 450

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D texNoise;

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

int kernelSize = 64;

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

const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);


void main() {

	float radius = 0.5;
	float bias = 0.025;

	vec3 fragPos = texture(gPosition, vec2(1.0 - uvCoords.x, uvCoords.y)).xyz;
	vec3 normal = normalize(texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y)).rgb);
	vec3 randomVec = normalize(texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz);

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i) {

		vec3 samplePos = TBN * kernelsamples.samples[i].xyz; // from tangent to view-space transform
		samplePos = fragPos + samplePos * radius; //add view-space kernel sample to view-space frag pos

		vec4 offset = vec4(samplePos, 1.0);
		offset = camera.proj * offset; //transform sample to clip space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0-1

		float sampleDepth = texture(gPosition, offset.xy).z; //retrieve the z (depth) value of the current position

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / kernelSize);

	outColor = occlusion;
}


