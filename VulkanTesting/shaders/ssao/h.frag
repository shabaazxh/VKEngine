#version 450
#define PI 3.1415926535897932384626433832795


layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;

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
	mat4 mvMatrix;
}kernelsamples;


layout(location = 3) in vec2 clip_XY;
layout(location = 1) in vec2 uvCoords;

layout(location = 0) out vec4 out_Color;


#define SAMPLING_RADIUS 0.5
#define NUM_SAMPLING_DIRECTIONS 6
#define SAMPLING_STEP 0.004
#define NUM_SAMPLING_STEPS 6
#define THRESHOLD 0.1
#define SCALE 1.0
#define TANGENT_BIAS 0.3

//steps 4 by default, radius is 0.5 by default

void main()
{
	float pos_z = texture(depthMap, vec2(1.0 - uvCoords.x, uvCoords.y)).r; //fragment shader runs for each pixel 
	vec3 pos_xy = vec3(vec2(1.0 - uvCoords.x, uvCoords.y), pos_z);
	vec3 ndc_Pos = (2.0 * pos_xy) - 1.0; // transform to normalized device coordinates xyz/w

	vec4 unproject = inverse(camera.proj) * vec4(ndc_Pos, 1.0);
	vec3 viewPos = unproject.xyz / unproject.w;

	vec3 viewNorm = normalize(texture(gNormal, vec2(1.0 - uvCoords.x, uvCoords.y))).rgb;

	float total = 0.0;
	float sample_direction_increment = 2 * PI / NUM_SAMPLING_DIRECTIONS; //split sampling sphere into multiple direction angles

	for(uint i = 0; i < NUM_SAMPLING_DIRECTIONS; i++) {
   
		float sampling_angle = i * sample_direction_increment;
		vec2 sampleDir = vec2(cos(sampling_angle), sin(sampling_angle));

		float tangentAngle = acos(dot(vec3(sampleDir, 0), viewNorm)) - (0.5 * PI) + TANGENT_BIAS;
		float horizonAngle = tangentAngle;
      vec3 lastDiff = vec3(0);

         for (uint j = 0; j < NUM_SAMPLING_STEPS; j++) {
            vec2 sampleOffset = float(j+1) * SAMPLING_STEP * sampleDir;
            vec2 offTex = vec2(1.0 - uvCoords.x, uvCoords.y) + sampleOffset;

            float off_start_Z = texture(depthMap, offTex.st).r;
            vec3 off_start_Pos = vec3(offTex, off_start_Z);
            vec3 off_ndc_Pos = (2.0 * off_start_Pos) - 1.0;
            vec4 off_unproject = inverse(camera.proj) * vec4(off_ndc_Pos, 1.0);
            vec3 off_viewPos = off_unproject.xyz / off_unproject.w;

            // we now have the view space position of the offset point
            vec3 diff = off_viewPos.xyz - viewPos.xyz;
            if (length(diff) < SAMPLING_RADIUS) {
                // skip samples which are outside of our local sampling radius
                lastDiff = diff;
                float elevationAngle = atan(diff.z / length(diff.xy));
                horizonAngle = max(horizonAngle, elevationAngle);
            }
        }

        float normDiff = length(lastDiff) / SAMPLING_RADIUS;
        float attenuation = 1 - normDiff*normDiff;

        float occlusion = clamp(attenuation * (sin(horizonAngle) - sin(tangentAngle)), 0.0, 1.0);
        total += 1.0 - occlusion;
	}

	total /= NUM_SAMPLING_DIRECTIONS;

	out_Color = vec4(total, total, total, 1.0);
}