#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;

layout (location = 0) out vec3 v_TexCoords;//WorldPos

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_TexCoords = a_Position;
    gl_Position = u_Projection * mat4(mat3(u_View)) * vec4(v_TexCoords, 1.0f);
}

#type fragment
#version 450 core
// Physically Based Rendering (Diffuse IBL)
// Converted from Michal Siejak's compute shader version

layout(location = 0) in vec3 v_TexCoords;
layout(location = 0) out vec4 o_FragColor;

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;
const float TwoPI = 2.0 * PI;
const float Epsilon = 1e-5;

const uint NumSamples = 64u * 1024u;
const float InvNumSamples = 1.0 / float(NumSamples);

// ----------------------------------------------------------------------
// Radical inverse / Hammersley sequence
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // 1 / 2pow32
}

vec2 SampleHammersley(uint i)
{
    return vec2(float(i) * InvNumSamples, RadicalInverse_VdC(i));
}

// ----------------------------------------------------------------------
// Uniform sampling on hemisphere
vec3 SampleHemisphere(float u1, float u2)
{
    float u1p = sqrt(max(0.0, 1.0 - u1 * u1));
    return vec3(cos(TwoPI * u2) * u1p, sin(TwoPI * u2) * u1p, u1);
}

// ----------------------------------------------------------------------
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

// ---------------------------------------------------------------------
// Main
void main()
{
    vec3 N = normalize(v_TexCoords);
    vec3 S, T;
    ComputeBasisVectors(N, S, T);

    // Monte Carlo integration over hemisphere
    // Integrates diffuse irradiance (Lambertian BRDF assumed)
    vec3 irradiance = vec3(0.0);

    for (uint i = 0u; i < NumSamples; ++i)
    {
        vec2 u = SampleHammersley(i);
        vec3 Li = TangentToWorld(SampleHemisphere(u.x, u.y), N, S, T);
        float cosTheta = max(dot(Li, N), 0.0);

        // PI terms cancel due to the Lambertian BRDF assumption
        irradiance += 2.0 * textureLod(u_EnvironmentMap, Li, 0.0).rgb * cosTheta;
    }

    irradiance /= float(NumSamples);
    o_FragColor = vec4(irradiance, 1.0);
}