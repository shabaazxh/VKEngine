#version 450

#define PI 3.1415926535897932384626433832795

layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;
layout(binding = 2) uniform sampler2D gNormal;

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
	vec4 camera_center;
	float z_far;
}kernelsamples;

layout(location = 0) out vec4 outColor;

vec4 ambient_occlusion;

const vec2 window = vec2(1920.0, 1080.0);
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

float near = 0.1;
float zfar = 1000;


float tc_depth(in vec2 tc)
{
	return texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y)).x;
}

float LinearizeDepth(in vec2 tc) {
	float buffer_z = texture(depthMap, tc).x;
	return (2.0 * near) / (zfar + near - buffer_z * (near - zfar));
}

float ndcDepth(in vec2 tc) {
	float depth = texture(depthMap, tc).x;

	return camera.proj[3][2] / (camera.proj[2][2] + depth);
}

float ec_depth(in vec2 tc)
{
	float buffer_z = texture(depthMap, tc).x;
	return camera.proj[3][2] / (-2.0 * buffer_z + 1.0 - camera.proj[2][2]);
}

void main() 
{
	vec2 tc_depths = gl_FragCoord.xy / window;
	
	// Normals
	vec3 normals = normalize(texture(gNormal, tc_depths)).rgb;

	// normals in world coordinates
	vec3 wc_normal = normalize(transpose(mat3(camera.view)) * normals);

	float ndc_linear_depth = -ec_depth(tc_depths) / zfar;
	float lindepth = LinearizeDepth(tc_depths);
	// camera_center(target) - camera_eye = world coordinate camera ray direction
	vec3 wc_position_current_frag = kernelsamples.camera_eye.xyz + normalize(( kernelsamples.camera_eye.xyz + kernelsamples.camera_center.xyz)) * ndc_linear_depth;

	ambient_occlusion.a = 0.0; 

	const int base_samples = 0;
	const int min_samples = 64;
	const float radius = 1.0;
	const float projection_factor = 0.75;
	const float bias = 1.000;
	const float sigma = 2.0;
	const float epsilon = 0.0001;

	int samples = max(int(base_samples / (1.0 + base_samples * ndc_linear_depth)), min_samples);

	mat4 inverse_view_projection_matrix = inverse(camera.view);
	float projected_radius = radius * projection_factor / -ec_depth(tc_depths);

	vec2 inverted_random_texture_size = 1.0 / vec2(textureSize(texNoise, 0));
	vec2 tc_random_texture = gl_FragCoord.xy * inverted_random_texture_size;

	// scale texture back to [-1: 1] range
	vec3 random_direction = texture(texNoise, tc_random_texture).xyz;
	random_direction = normalize(random_direction * 2.0 - 1.0);

	for(int i = 0; i < samples; i++) 
	{
		// select random point
		vec2 sample_random_direction = texture(texNoise,  vec2(float(i) * inverted_random_texture_size.x, float(i / textureSize(texNoise, 0).x) * inverted_random_texture_size.y)).xy;
		sample_random_direction = sample_random_direction * 2.0 - 1.0; //to screen coordinates

		//read position and depth for sample
		vec3 tc_sample;
		tc_sample.xy = tc_depths + sample_random_direction * projected_radius;
		tc_sample.z = tc_depth(tc_sample.xy);

		vec3 ndc_sample = tc_sample * 2.0 - 1.0;
		vec4 temporary = inverse_view_projection_matrix * vec4(ndc_sample, 1.0);
		vec3 wc_sample = temporary.xyz / temporary.w;

		vec3 v = wc_sample - wc_position_current_frag;

		ambient_occlusion.a += max(0.0, dot(v, wc_normal) - bias) / (dot(v, v) + epsilon);
	}

	ambient_occlusion.a = max(0.0, 1.0 - 2.0 * sigma / float(samples) * ambient_occlusion.a);

	outColor = vec4(ambient_occlusion.a, ambient_occlusion.a, ambient_occlusion.a, 1.0); 

}