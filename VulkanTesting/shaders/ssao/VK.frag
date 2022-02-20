#version 450

// Shader by Vladamir Kajalin - ShaderX7 

layout(binding = 3) uniform sampler2D texNoise;
layout(binding = 6) uniform sampler2D depthMap;

layout(location = 0) out vec4 outColor;

const vec2 window = vec2(1920.0, 1080.0);

float ComputeSSAO(sampler2D sRotSampler4x4, sampler2D sSceneDepthSampler, vec2 screenTC, vec2 screenSize, float farClipDist) 
{
	vec2 rotationTC = screenTC * screenSize / 4;
	vec3 vRotation = 2 * texture(sRotSampler4x4, gl_FragCoord.xy/4.0).rgb * 2.0 - 1.0;

	mat3 rotMat;
	float h = 1 / ( 1 + vRotation.z);
	rotMat[0][0] = h * vRotation.y * vRotation.y + vRotation.z;
	rotMat[0][1] = -h * vRotation.y * vRotation.x;
	rotMat[0][2] = -vRotation.x;
	rotMat[1][0] = -h * vRotation.y * vRotation.x;
	rotMat[1][1] = h * vRotation.x * vRotation.x + vRotation.z;
	rotMat[1][2] = -vRotation.y; rotMat[2][0] = vRotation.x;
	rotMat[2][1] = vRotation.y; rotMat[2][2] = vRotation.z;


	float fSceneDepthP = texture(sSceneDepthSampler, screenTC).r * farClipDist;

	const int nSamplesNum = 64;
	float offsetScale = 0.01;
	const float offsetScaleStep = 1 + 2.4/nSamplesNum;

	float Accessibility = 0;

	for(int i = 0; i < (nSamplesNum/8); i++)
	for(int x = -1; x <=1; x+=2)
	for(int y = -1; y<=1; y+=2)
	for(int z = -1; z<=1; z+=2) {

		vec3 vOffset = normalize(vec3(x,y,z)) * (offsetScale *= offsetScaleStep);

		vec3 vRotatedOffset = rotMat * vOffset;

		vec3 vSamplePos = vec3(screenTC, fSceneDepthP);

		vSamplePos += vec3(vRotatedOffset.xy, vRotatedOffset.z * fSceneDepthP * 2);

		float fSceneDepthS = texture(sSceneDepthSampler, vSamplePos.xy).z * farClipDist;

		float fRangeIsInvalid = clamp((fSceneDepthP - fSceneDepthS)/fSceneDepthS, 0.0, 1.0);
		Accessibility += mix((float(fSceneDepthS > vSamplePos.z)), 0.5, fRangeIsInvalid);
	}

	Accessibility = Accessibility / nSamplesNum;

	return clamp(Accessibility * Accessibility + Accessibility, 0.0, 1.0);
}

void main ()
{
	vec2 screenTC = gl_FragCoord.xy / window;
	float ssao = ComputeSSAO(texNoise, depthMap, screenTC, window, 1000);
	outColor = vec4(ssao,ssao,ssao,1.0);
}