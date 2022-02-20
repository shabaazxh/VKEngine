
#version 450

const float PI = 3.14159265359;

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;
layout(binding = 2) uniform sampler2D gNormal;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvCoords;
#define FAR_PLANE -1000.f


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

layout(location = 0) out vec2 outColor;

const vec2 window = vec2(1920.0, 1080.0);
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

int n_samples = 64;
int turns = 12;
float ball_radius = kernelsamples.radius; //0.425
float sigma = kernelsamples.shadowScalar;
float kappa = kernelsamples.shadowContrast;
float beta = 0.0005f;
float epsilon = 0.0001f;

void main(void){
	vec3 pos = texture(gPosition, gl_FragCoord.xy / window).xyz;
	vec3 normal = normalize(texture(gNormal, gl_FragCoord.xy / window).xyz);

	// The Alchemy AO hash for random per-pixel offset
	ivec2 px = ivec2(gl_FragCoord.xy);
	float phi = (30u * px.x ^ px.y + px.x * px.y) * 10u;
	const float TAU = 6.2831853071795864;
	const float ball_radius_sqr = pow(ball_radius, 2);
	// What's the radius of a 1m object at z = -1m to compute screen_radius properly?
	// Comments in their code mention we can compute it from the projection mat, or hardcode in like 500
	// and make the ball radius resolution dependent (as I've done currently)
	const float screen_radius = -ball_radius * 100.0 / pos.z;
	int max_mip = textureQueryLevels(gPosition) - 1;
	float ao_value = 0;
	for (int i = 0; i < n_samples; ++i){
		
		float alpha = 1.f / n_samples * (i + 0.5);
		float h = screen_radius * alpha;
		float theta = TAU * alpha * turns + phi;
		vec2 u = vec2(cos(theta), sin(theta));
		int m = clamp(findMSB(int(h)) - 4, 0, max_mip);
		ivec2 mip_pos = clamp((ivec2(h * u) + px) >> m, ivec2(0), textureSize(gPosition, m) - ivec2(1));
		vec3 q = texelFetch(gPosition, mip_pos, m).xyz;
		vec3 v = q - pos;

		// The original estimator in the paper, from Alchemy AO
		// I tried getting their new recommended estimator running but couldn't get it to look nice,
		// from taking a look at their AO shader it also looks like we compute this value quite differently
		ao_value += max(0, dot(v, normal + pos.z * beta)) / (dot(v, v) + epsilon);
	}
	// The original method in paper, from Alchemy AO
	ao_value = max(0, 1.f - 2.f * sigma / n_samples * ao_value);
	ao_value = pow(ao_value, kappa);

    // Do a little bit of filtering now, respecting depth edges
    if (abs(dFdx(pos.z)) < 0.02) {
        ao_value -= dFdx(ao_value) * ((px.x & 1) - 0.5);
    }
    if (abs(dFdy(pos.z)) < 0.02) {
        ao_value -= dFdy(ao_value) * ((px.y & 1) - 0.5);
    }
	outColor = vec2(ao_value, pos.z / FAR_PLANE);
}