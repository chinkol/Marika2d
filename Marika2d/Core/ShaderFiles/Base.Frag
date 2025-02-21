#version 430 core

in vec3 normal;
in vec2 texCoord;
	
layout(location = 0) out vec4 color;
layout(location = 1) out uvec2 outId;

uniform uvec2 objectId;
uniform sampler2D diffuseMap;

void main()
{
	color = texture(diffuseMap, texCoord);
	outId = objectId;
}









struct Light
{
	vec3 color;
	float intensity;
	vec3 direction;
	float attenuation;
	vec3 position;
	float innerAngle;
	float outerAngle;
	int lightType;
};

struct Material
{
    vec4 albedo;
    vec3 fresnel;
    float shininess;
};
