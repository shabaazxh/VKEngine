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
}kernelsamples;

float near = 0.1;
float zfar = 1000;

layout(location = 0) out vec4 outColor;

vec4 ambient_occlusion;

float LinearizeDepth(in vec2 tc) {
	float buffer_z = texture(depthMap, tc).x;
	return (2.0 * near) / (zfar + near - buffer_z * (near - zfar));
}

float ec_depth(in vec2 tc)
{
	float buffer_z = texture(depthMap, tc).x;
	return camera.proj[3][2] / (2.0 * buffer_z - 1.0 - camera.proj[2][2]);
}

const vec2 window = vec2(1920.0, 1080.0);


const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

int samples = 64;
float radius = 0.5;

void main() {

	vec3 pos = texture(gPosition, vec2(1.0 - uvCoords.x, uvCoords.y)).xyz;
	//vec3 randomVec = normalize(texture(texNoise, tc_random_texture * noiseScale).xyz * 2.0 - 1.0);

	float AO = 0.0;

	for(int i = 0; i < samples; i++){

        vec3 samplePos = pos + kernelsamples.samples[i] * radius; // generate a random point
        vec4 offset = vec4(samplePos, 1.0); // make it a 4-vector
        offset = camera.proj * offset; // project on the near clipping plane
        offset.xy /= offset.w; // perform perspective divide
        offset.xy = offset.xy * 0.5 + 0.5; // transform to (0,1) range

        float sampleDepth = texture(gPosition, offset.xy).z;

        if (abs(pos.z - sampleDepth) < 0.5) {
            AO += step(sampleDepth, samplePos.z);
        }
    }

    AO = 1.0 - AO/samples;

    outColor = vec4(pow(AO, 2.0));

}