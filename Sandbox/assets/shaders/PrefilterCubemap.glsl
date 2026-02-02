#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;

layout (location = 0) out vec3 v_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_TexCoords = a_Position;
    gl_Position = u_Projection * mat4(mat3(u_View)) * vec4(a_Position, 1.0f);
}

#type fragment
#version 450 core

// Based on Michal Siejak�s GGX importance-sampling prefilter
// Outputs prefiltered cubemap color per fragment

layout(location = 0) in vec3 v_TexCoords; // direction (world-space or cube dir)
layout(location = 0) out vec4 o_FragColor;

uniform float u_Roughness;
uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;
const float TwoPI = 2.0 * PI;
const float Epsilon = 1e-5;
const uint NumSamples = 1024u;
const float InvNumSamples = 1.0 / float(NumSamples);

// ----------------------------------------------------
// Radical inverse and Hammersley sampling
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 SampleHammersley(uint i)
{
    return vec2(float(i) * InvNumSamples, RadicalInverse_VdC(i));
}

// ----------------------------------------------------
// GGX importance sampling (half-vector)
vec3 SampleGGX(float u1, float u2, float roughness)
{
    float alpha = roughness * roughness;
    float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha * alpha - 1.0) * u2));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float phi = TwoPI * u1;
    return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

// ----------------------------------------------------
// GGX NDF
float NDF_GGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// ----------------------------------------------------
// Compute orthonormal basis
void ComputeBasisVectors(in vec3 N, out vec3 S, out vec3 T)
{
    T = cross(N, vec3(0.0, 1.0, 0.0));
    if (dot(T, T) < Epsilon)
        T = cross(N, vec3(1.0, 0.0, 0.0));
    T = normalize(T);
    S = normalize(cross(N, T));
}

vec3 TangentToWorld(vec3 v, vec3 N, vec3 S, vec3 T)
{
    return S * v.x + T * v.y + N * v.z;
}

// ----------------------------------------------------
// Main
void main()
{
    vec3 N = normalize(v_TexCoords);
    vec3 Lo = N; // Assume view direction aligned with normal

    vec3 S, T;
    ComputeBasisVectors(N, S, T);

    // Solid angle per texel at mip 0
    vec2 envSize = vec2(textureSize(u_EnvironmentMap, 0));
    float wt = 4.0 * PI / (6.0 * envSize.x * envSize.y);

    vec3 color = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < NumSamples; ++i)
    {
        vec2 u = SampleHammersley(i);
        vec3 Lh = TangentToWorld(SampleGGX(u.x, u.y, u_Roughness), N, S, T);
        vec3 Li = normalize(2.0 * dot(Lo, Lh) * Lh - Lo);

        float cosLi = max(dot(N, Li), 0.0);
        if (cosLi > 0.0)
        {
            float cosLh = max(dot(N, Lh), 0.0);
            float pdf = NDF_GGX(cosLh, u_Roughness) * 0.25;
            float ws = 1.0 / (float(NumSamples) * pdf + Epsilon);
            float mipLevel = u_Roughness == 0.0 ? 0.0 : max(0.5 * log2(ws / wt) + 1.0, 0.0);

            color += textureLod(u_EnvironmentMap, Li, mipLevel).rgb * cosLi;
            totalWeight += cosLi;
        }
    }

    color = color / max(totalWeight, Epsilon);
    o_FragColor = vec4(color, 1.0);
}
