#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

out vec3 v_WorldDir;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_WorldDir = a_Position;

    // Remove translation from view matrix (we only need rotation for cubemap rendering)
    mat4 rotView = mat4(mat3(u_View));

    gl_Position = u_Projection * rotView * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core
in vec3 v_WorldDir;
out vec4 o_Color;

uniform sampler2D uEquirectangularMap;

const float PI = 3.14159265359;

vec2 DirToEquirectUV(vec3 dir)
{
    dir = normalize(dir);
    float phi = atan(dir.z, dir.x);
    float theta = acos(dir.y);

    vec2 uv = vec2(phi / (2.0 * PI) + 0.5, theta / PI);
    // Flip vertically image upside down
    uv.y = 1.0 - uv.y;
    return uv;
}

void main()
{
    vec3 dir = normalize(v_WorldDir);
    vec2 uv = DirToEquirectUV(dir);
    vec3 color = texture(uEquirectangularMap, uv).rgb;
    o_Color = vec4(color, 1.0);
}