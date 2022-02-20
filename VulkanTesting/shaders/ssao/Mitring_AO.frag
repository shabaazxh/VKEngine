#version 450

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

void main() {

	vec2 tc_depths = gl_FragCoord.xy / window;

	float ec_depth_negated = LinearizeDepth(tc_depths);

	ambient_occlusion.a = 0.0f;
	const float radius = 10.0f;
	const int samples = 64;

	float projection_scale_xy = ec_depth_negated;
	float projection_scale_z = 1000.0 / zfar * projection_scale_xy;

	float scene_depth = texture(depthMap, tc_depths).x;

	vec2 inverted_random_texture_size = 1.0 / vec2(textureSize(texNoise, 0));
	vec2 tc_random_texture = gl_FragCoord.xy * inverted_random_texture_size;

	// here we are converting the random vectors from the range - 1 to 1
	//vec3 random_direction = texture(texNoise, tc_random_texture).xyz;
	//random_direction = normalize(random_direction * 2.0 - 1.0); //NDC

	vec3 randomVec = normalize(texture(texNoise, tc_random_texture * noiseScale).xyz * 2.0 - 1.0);

	for(int i = 0; i < samples; i++)
	{
		vec3 sample_random_direction = texture(texNoise, vec2(float(i) * 
		inverted_random_texture_size.x, float(i / textureSize(texNoise, 0).x) *
		inverted_random_texture_size.y)).xyz;

		sample_random_direction = sample_random_direction * 2.0 - 1.0;
		sample_random_direction = reflect(sample_random_direction, randomVec);

		vec3 tc_sample_pos = vec3(tc_depths.xy, scene_depth) 
				 + vec3(sample_random_direction.xy * projection_scale_xy,
				 	 sample_random_direction.z * scene_depth * projection_scale_z) * radius;

		float sample_depth = texture(depthMap, tc_sample_pos.xy).x;

		ambient_occlusion.a += float(sample_depth > tc_sample_pos.z);
	}

	
	ambient_occlusion.a /= float(samples);
	outColor = vec4(ambient_occlusion.a, ambient_occlusion.a,ambient_occlusion.a, 1.0);

}