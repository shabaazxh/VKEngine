#version 450


layout(binding = 4) uniform sampler2D depthMap;

layout(location = 4) in vec2 fragTexCoord;

layout(location = 0) out vec4 FragColor;

void main(){
	/*float depthValue = texture(depthMap, fragTexCoord).r;
	FragColor = vec4(vec3(depthValue), 1.0);
	gl_FragDepth = gl_FragCoord.z;*/
}