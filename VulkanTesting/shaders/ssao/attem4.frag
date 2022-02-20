#version 450

#define PI 3.1415926535897932384626433832795

layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;
layout(binding = 1) uniform sampler2D gPosition;

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

const vec2 noiseScale = vec2(1920/4.0, 1080/4.0);

const float R2 = 1.0 * 1.0;
const float inverseR2 = -1.0f / R2;
const float NUM_STEPS = 4;
const float NUM_DIRECTIONS = 8;
const float bias = 0.3f;

vec4 depthToPosition(vec2 uv) {
    float depth = texture(depthMap, uv).x;

    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpace = inverse(camera.proj) * clipSpace;
    viewSpace.xyz /= viewSpace.w;

    return vec4(vec3(viewSpace), 1.0);
}

vec4 depthToNormal(vec2 tc)
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

vec2 RotateDirection(vec2 Dir, vec2 CosSin)
{
    return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
        Dir.x*CosSin.y + Dir.y*CosSin.x);
}


float Falloff(float DistanceSquare)
{
    return DistanceSquare * inverseR2 + 1.0;
}

float ComputeAO(vec3 P, vec3 N, vec3 S)
{
  vec3 V = S - P;
  float VdotV = dot(V, V);
  float NdotV = dot(N, V) * 1.0/sqrt(VdotV);

  // Use saturate(x) instead of max(x,0.f) because that is faster on Kepler
  return clamp(NdotV -bias,0,1) * clamp(Falloff(VdotV),0,1);
}

vec4 GetJitter()
{
    return textureLod(texNoise, (gl_FragCoord.xy / 4), 0);
}

float ComputeCoarseAO(vec2 FullResUV, float RadiusPixels, vec4 Rand, vec3 ViewPosition, vec3 ViewNormal)
{

  // Divide by NUM_STEPS+1 so that the farthest samples are not fully attenuated
  float StepSizePixels = RadiusPixels / (NUM_STEPS + 1);

  const float Alpha = 2.0 * PI / NUM_DIRECTIONS;
  float AO = 0;

  for (float DirectionIndex = 0; DirectionIndex < NUM_DIRECTIONS; ++DirectionIndex)
  {
    float Angle = Alpha * DirectionIndex;

    // Compute normalized 2D direction
    vec2 Direction = RotateDirection(vec2(cos(Angle), sin(Angle)), Rand.xy);

    // Jitter starting sample within the first step
    float RayPixels = (Rand.z * StepSizePixels + 1.0);

    for (float StepIndex = 0; StepIndex < NUM_STEPS; ++StepIndex)
    {
      vec2 SnappedUV = round(RayPixels * Direction) * vec2(1.0f/float(1920), 1.0f/(1080)) + FullResUV;
      vec3 S = texture(gPosition, SnappedUV).xyz;

      RayPixels += StepSizePixels;

      AO += ComputeAO(ViewPosition, ViewNormal, S);
    }
  }

  AO *= (1.0f / (1.0f - bias)) / (NUM_DIRECTIONS * NUM_STEPS);
  return clamp(1.0 - AO * 2.0,0,1);
}

float sum = 0.0;
float occlusion = 0.0;

void main()
{



}