#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

out vec3 v_Position;//WorldPos

layout (std140, binding = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_ViewProjection;

    vec4 u_CameraPosition;
};

void main()
{
	v_Position = a_Position;

	mat4 rotView = mat4(mat3(u_View));
	//gl_Position = u_Projection * rotView * vec4(v_Position, 1.0);
	vec4 clipPos = u_Projection * rotView * vec4(v_Position, 1.0);

	gl_Position = clipPos.xyww;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 oFragColor;

in vec3 v_Position;

uniform samplerCube uEnvironmentMap;

void main()
{
	vec3 envColor = textureLod(uEnvironmentMap, v_Position, 0).rgb;
	// simple Reinhard
	envColor = envColor / (envColor + vec3(1.0));
	oFragColor = vec4(envColor, 1.0);

}
