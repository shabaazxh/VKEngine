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

layout(location = 0) out vec4 outColor;

float sampleRadius = ssaoparams.radius;
int samples = ssaoparams.sampleAmount; 
int turns = 16;
float depthThreshold = 0.000;
float shadowScalar = ssaoparams.shadowScalar; 
float shadowContrast = ssaoparams.shadowContrast;
const float epsilon = 0.0001;

void main()
{
	// transform position to view space
	vec3 worldFragmentPos = texture(gPosition, vec2(1.0 - uvCoords.x, uvCoords.y)).xyz;
	// Convert world position to view-space position
	vec3 Position = vec3(camera.view * vec4(worldFragmentPos, 1.0));

	// normals in view-space from g-buffer texture
	vec3 Normal = normalize(texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y))).xyz;

	float ambientValue = 0.0;
	float screen_radius = (sampleRadius * 100.0 / Position.z);
	
	int max_mip = textureQueryLevels(gPosition) - 1;
	const float TAU = 2.0 * PI;
	ivec2 px = ivec2(gl_FragCoord.xy);

	//(Usher, 2015)
	float phi = (30u * px.x ^ px.y + 10u * px.x * px.y);
	
	for (int i = 0; i < samples; ++i)
	{
		// Define Alchemy AO helper variables (McGuire, Mara and Luebke, 2012)
		float alpha = 1.f / samples * (i + 0.5);
		float h = screen_radius * alpha;
		float theta = TAU * alpha * turns + phi;
		vec2 u = vec2(cos(theta), sin(theta));

		// McGure MIP calculation (McGuire, Mara and Luebke, 2012)
		int m = clamp(findMSB(int(h)) - 4, 0, max_mip);
		ivec2 mip_pos = clamp((ivec2(h * u) + px) >> m, ivec2(0), textureSize(gPosition, m) - ivec2(1));

		//(Usher, 2015)
		//vec3 worldPi = texelFetch(gPosition, mip_pos, m).xyz;
		vec3 Pi = texelFetch(gPosition, mip_pos, m).xyz;
		//vec3 Pi = vec3(camera.view * vec4(worldPi, 1.0));
		vec3 V = Pi - Position;
		float sqrLen    = dot(V, V);
		float Heaviside = step(length(sqrLen), sampleRadius);
		float dD = depthThreshold * Position.z;
		float c = 0.1 * sampleRadius;
								  
		ambientValue += (max(0.0, dot(Normal, V) + dD) * Heaviside) / (max(c * c, sqrLen) + epsilon);
	  
	}
	
	ambientValue *= (2.0 * shadowScalar) / samples;
	ambientValue = max(0.0, 1.0 - pow(ambientValue, shadowContrast));
	
    outColor = vec4(ambientValue, ambientValue, ambientValue, 1.0);
}


