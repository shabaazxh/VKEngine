#version 450

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D texNoise;

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

layout(location = 0) out float outColor;

const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

void main() {

	float radius = 0.5;
	float bias = 0.025;

	vec3 fragPos = texture(gPosition, vec2(1.0 - uvCoords.x, uvCoords.y)).xyz;
	vec3 normal = texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y)).rgb;
	vec3 randomVec = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i) {

		vec3 samplePos = TBN * kernelsamples.samples[i];
		samplePos = fragPos + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.0);
		offset = camera.proj * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(gPosition, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / kernelSize);

	outColor = occlusion;
}


