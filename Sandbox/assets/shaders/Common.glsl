#pragma once

const float PI = 3.14159265;
const float Epsilon = 1e-5;

vec4 sRGBtoLinear(vec4 color)
{
	return pow(color, vec4(vec3(2.2), 1.0));
}

layout(std140, binding = 0) uniform Camera
{
	mat4 uView;
	mat4 uProjection;
	mat4 uViewProjection;
	vec4 uCameraPosition;
};

struct DirectionalLight
{
	vec4 uPosition;
	vec4 uColor;
	vec4 uLightDir;
};

layout(std140, binding = 1) uniform DirectionalLightBuffer
{
	DirectionalLight uDirectionalLight; // single instance
};