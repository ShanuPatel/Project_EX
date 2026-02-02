#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 uView;
	mat4 uProjection;
	mat4 uViewProjection;

	vec4 uCameraPosition;
};

uniform mat4 uModel;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
	vTexCoord = aTexCoord;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 oFragColor;

in vec2 vTexCoord;

uniform sampler2D uTexture;
void main()
{
	oFragColor = texture(uTexture, vTexCoord);
	//oFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}