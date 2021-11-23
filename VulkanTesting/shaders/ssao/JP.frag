#version 450


layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;


layout(binding = 4) uniform KernelSample {
	vec3 samples[64];
	mat4 projection;
	vec4 camera_eye;
	vec4 camera_direction;
	float z_far;
}kernelsamples;


const vec2 noiseScale = vec2(2560.0/4.0, 1440.0/4.0);

vec3 computeViewPosFromDepth(vec2 texcoords) {
	
	float depth = texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y)).x;

	vec4 clipSpace = vec4(vec2(1.0 - uvCoords.x, uvCoords.y) * 2.0 - 1.0, depth, 1.0);
	vec4 viewSpace = inverse(camera.proj) * clipSpace;
	return viewSpace.xyz /= viewSpace.w;
}



int kernelSize = 64;

layout(location = 0) out float outColor;


void main()
{

	float radius = 0.5;
	float bias = 0.025;

	vec3 origin = computeViewPosFromDepth(vec2(1.0 - uvCoords.x, uvCoords.y));
	vec3 normal = texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y)).rgb;
	vec3 randomVec = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz;

	normal = normalize(normal);

	vec3 rvec = texture(texNoise, uvCoords * noiseScale).xyz * 2.0 - 1.0;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;

	for(int i = 0; i < kernelSize; i++) {

		vec3 samplePos = tbn * kernelsamples.samples[i];
		samplePos = samplePos * radius + origin;

		//project sample position
		vec4 offset = vec4(samplePos, 1.0);
		offset = camera.proj * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		// get sample depth
		float sampleDepth = texture(depthMap, offset.xy).r;

		// range check
		float rangeCheck = abs(origin.z - sampleDepth) < radius ? 1.0 : 0.0;
		occlusion += (sampleDepth <= samplePos.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 / (occlusion/kernelSize);

	outColor = occlusion;

}