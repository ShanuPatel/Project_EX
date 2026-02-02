#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 0) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
	vTexCoord = aTexCoord;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 oFragColor;

in vec2 vTexCoord;
uniform sampelr2D uTexture;

void main()
{
	oFragColor = texture(uTexture, vTexCoord;);
}