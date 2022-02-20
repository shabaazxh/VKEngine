#version 450

layout(binding = 3) uniform sampler2D texNoise;
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
    float ambientLightLevel;
}kernelsamples;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out vec4 outColor;

float zNear = 0.1;
float zFar = 1000;

const vec3 offsets[8] = vec3[8]
( 
    vec3(-0.5,-0.5,-0.5), 
    vec3( 0.5,-0.5,-0.5), 
    vec3(-0.5, 0.5,-0.5), 
    vec3( 0.5, 0.5,-0.5), 
    vec3(-0.5, -0.5, 0.5), 
    vec3(0.5,-0.5, 0.5), 
    vec3(-0.5, 0.5, 0.5), 
    vec3(0.5, 0.5, 0.5)
);

const int samples = 16;
vec3 sample_sphere[samples] = {
      vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
      vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
      vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
      vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
      vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
      vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
      vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
      vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
  };

const vec2 noiseScale = vec2(1920/4.0, 1080/4.0);

vec4 viewSpace(vec2 uv){
    float depth = texture(depthMap, uv).x;

    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpace = inverse(camera.proj) * clipSpace;
    viewSpace.xyz /= viewSpace.w;

    return vec4(vec3(viewSpace), 1.0);
}

//0.027 is the preferred radius
void main()
{

    float radius = kernelsamples.radius;
    float bias = 0.2; //removing banding 0.2
    float scale = 1.0; //1.0
    float distScale = 0.5; //0.5
    float attScale = 0.5; //1.0
    float ambientLightLevel = kernelsamples.ambientLightLevel;

    // z position of the fragment from the screen
    float Z = (zFar * zNear) / (texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y)).x * 
        (zFar-zNear) - zFar);

    vec3 plane = texture(texNoise, vec2(1.0 - uvCoords.x, uvCoords.y) * noiseScale).xyz - vec3(1.0);

    float occlusion = 0.0;

    for(int i = 0; i < kernelsamples.sampleAmount; i++) {

        vec3 rSample = reflect(kernelsamples.samples[i].xyz, plane);

        float zSample = texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y) + 
            rSample.xy * radius / Z).x;

        //converted to eye-space
        zSample = (zFar*zNear) / (zSample * (zFar-zNear) - zFar);


        float inRange = step(zSample - Z, radius);
        occlusion += (zSample >= Z ? 1.0 : 0.0) * inRange;

    }

    occlusion = 1.0 - (occlusion / kernelsamples.sampleAmount);
    outColor = vec4(vec3(occlusion), 1.0);

}