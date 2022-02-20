#version 450

const float PI = 3.14159265359;

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;
layout(binding = 2) uniform sampler2D gNormal;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;


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

layout(location = 0) out vec4 outColor;

const vec2 window = vec2(1920.0, 1080.0);
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

float sampleRadius = kernelsamples.radius;//3.348; // //1.0
int samples = 12; //12
int sampleTurns = 16;
float depthThreshold = 0.000;
float shadowScalar = kernelsamples.shadowScalar; //; //1.0
float shadowContrast = kernelsamples.shadowContrast;//; //0.5
const float epsilon = 0.0001;

float randAngle()
{
	uint x = uint(gl_FragCoord.x);
	uint y = uint(gl_FragCoord.y);
	return (30u * x ^ y + 10u * x * y);
}

void main()
{
	// transform position to view space
	vec3 worldPos = texture(gPosition, vec2(1.0 - uvCoords.x, uvCoords.y)).xyz;
	vec3 P = vec3(camera.view * vec4(worldPos, 1.0));

	// normals in view-space from g-buffer texture
	vec3 N = normalize(texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y))).xyz;
	//N.xyz = -N.xyz;

	float aoValue = 0.0;
	float perspectiveRadius = (sampleRadius * 100.0 / P.z);
	
	int max_mip = textureQueryLevels(gPosition) - 1;
	const float TAU = 2.0 * PI;
	ivec2 px = ivec2(gl_FragCoord.xy);
	
	// Perform random sampling and estimate ambient occlusion for the current fragment
	for (int i = 0; i < samples; ++i)
	{
		// Alchemy helper variables
		float alpha = 1.f / samples * (i + 0.5);
		float h = perspectiveRadius * alpha;
		float theta = TAU * alpha * sampleTurns + randAngle();
		vec2 u = vec2(cos(theta), sin(theta));

		// McGuire paper MIP calculation
		int m = clamp(findMSB(int(h)) - 4, 0, max_mip);
		ivec2 mip_pos = clamp((ivec2(h * u) + px) >> m, ivec2(0), textureSize(gPosition, m) - ivec2(1));

		vec3 worldPi = texelFetch(gPosition, mip_pos, m).xyz;
		vec3 Pi = vec3(camera.view * vec4(worldPi, 1.0));
		vec3 V = Pi - P;
		float sqrLen    = dot(V, V);
		float Heaviside = step(length(sqrLen), sampleRadius);
		float dD = depthThreshold * P.z;
		float c = 0.1 * sampleRadius;
								  
		// Summation of the obscurance 
		aoValue += (max(0.0, dot(N, V) + dD) * Heaviside) / (max(c * c, sqrLen) + epsilon);
	  
	}
	
	// Final scalar multiplications for averaging and intensifying shadowing
	aoValue *= (2.0 * shadowScalar) / samples;
	aoValue = max(0.0, 1.0 - pow(aoValue, shadowContrast));
	
    outColor = vec4(aoValue, aoValue, aoValue, 1.0);
}


