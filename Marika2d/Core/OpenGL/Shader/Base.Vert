#version 430 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec3 normal;
out vec2 texCoord;

// pass constant
layout (location = 0) uniform mat4 view;
layout (location = 4) uniform mat4 proj;
layout (location = 8) uniform mat4 viewproj;

// object constant
layout (location = 12) uniform mat4 world;
		
void main()
{
	gl_Position = viewproj * world * vec4(inPos.x, inPos.y, inPos.z, 1.0f);
	normal = inNormal;
	texCoord = inTexCoord;
}
