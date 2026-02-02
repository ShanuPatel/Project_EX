#type compute
#version 450 core

const float PI = 3.14159265359;
const float TwoPI = 2.0 * PI;

layout(binding = 0) uniform sampler2D inputTexture;
layout(binding = 1, rgba16f) restrict writeonly uniform imageCube outputTexture;

vec3 getSamplingVector(ivec2 px, int face, ivec2 size)
{
    vec2 st = vec2(px) / vec2(size);
    vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0, 0.0);

    vec3 dir;
    if (face == 0)      dir = vec3(1.0, uv.y, -uv.x);
    else if (face == 1) dir = vec3(-1.0, uv.y, uv.x);
    else if (face == 2) dir = vec3(uv.x, 1.0, -uv.y);
    else if (face == 3) dir = vec3(uv.x, -1.0, uv.y);
    else if (face == 4) dir = vec3(uv.x, uv.y, 1.0);
    else                dir = vec3(-uv.x, uv.y, -1.0);

    return normalize(dir);
}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main()
{
    uvec3 gid = gl_GlobalInvocationID;

    int size = imageSize(outputTexture).x;   // ? fixed
    ivec2 size2 = ivec2(size);

    ivec2 pix = ivec2(int(gid.x), int(gid.y));
    int face = int(gid.z);

    if (pix.x >= size2.x || pix.y >= size2.y || face >= 6)
        return;

    vec3 dir = getSamplingVector(pix, face, size2);

    float phi = atan(dir.z, dir.x);
    float theta = acos(clamp(dir.y, -1.0, 1.0));
    vec2 equirectUV = vec2(phi / TwoPI + 0.5, theta / PI);

    vec4 color = texture(inputTexture, equirectUV);
    imageStore(outputTexture, ivec3(pix, face), color);
}