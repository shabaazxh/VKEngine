#version 450

layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;

layout(binding = 5) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
}camera;

layout(binding = 4) uniform KernelSample {
	vec3 samples[32];
	mat4 projection;
}kernelsamples;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out vec4 outColor;

float zNear = 0.1;
float zFar = 1000;

int kernelSize = 32;
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

const vec3 offsets[8] = vec3[8]
( vec3(-0.5,-0.5,-0.5), vec3( 0.5,
-0.5,-0.5), vec3(-0.5, 0.5,-0.5), vec3( 0.5,
0.5,-0.5), vec3(-0.5, -0.5, 0.5), vec3(
0.5,-0.5, 0.5), vec3(-0.5, 0.5, 0.5), vec3(
0.5, 0.5, 0.5));



void main()
{
    float radius = 0.05;
    float bias = 0.2; //removing banding
    float scale = 1.0;
    float distScale = 0.2;
    float attScale = 1.0;

    float size = 1.0 / float(textureSize(depthMap, 0).x);
    float Z = (zFar * zNear) / (texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y)).x * 
        (zFar-zNear) - zFar);

    vec3 plane = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz - vec3(1.0);
    float att = 0.0;


    for(int i = 0; i < 8; i++) {
        vec3 rSample = reflect(offsets[i], plane);

        float zSample = texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y) + 
            radius * rSample.xy / Z).x;

        zSample = (zFar*zNear) / (zSample * (zFar-zNear) - zFar);

        float dist = max(zSample - Z, 0.0) / distScale;

        float occl = 15.0 * max(dist * (2.0 - dist), 0.0);

        att += 1.0 / (1.0 + occl * occl);

    }

    att = clamp(att*att/64. + 0.45, 0., 1.) * attScale;

    outColor = vec4(vec3(att), 1.0);

}