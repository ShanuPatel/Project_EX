#type vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 1.0);
}

#type fragment
#version 450 core
out vec2 FragColor;
in vec2 TexCoords;

const float PI = 3.141592;
const float TwoPI = 2.0 * PI;
const float Epsilon = 0.001;
const uint NumSamples = 1024u;
const float InvNumSamples = 1.0 / float(NumSamples);

// ----------------------------------------------------
// Radical inverse using Van der Corput sequence
float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// ----------------------------------------------------
// Hammersley sequence
vec2 SampleHammersley(uint i)
{
	return vec2(float(i) * InvNumSamples, RadicalInverse_VdC(i));
}

// ----------------------------------------------------
// GGX importance sampling
vec3 SampleGGX(float u1, float u2, float roughness)
{
	float alpha = roughness * roughness;
	float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha * alpha - 1.0) * u2));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	float phi = TwoPI * u1;
	return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

// ----------------------------------------------------
// Geometry term (Schlick-GGX)
float GaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float GaSchlickGGX_IBL(float cosLi, float cosLo, float roughness)
{
	float r = roughness;
	float k = (r * r) / 2.0;
	return GaSchlickG1(cosLi, k) * GaSchlickG1(cosLo, k);
}

// ----------------------------------------------------
// Main
void main()
{
	// Map texture coordinates
	float cosLo = TexCoords.x;
	float roughness = TexCoords.y;

	cosLo = max(cosLo, Epsilon);
	vec3 Lo = vec3(sqrt(1.0 - cosLo * cosLo), 0.0, cosLo);

	float DFG1 = 0.0;
	float DFG2 = 0.0;

	for (uint i = 0u; i < NumSamples; ++i)
	{
		vec2 u = SampleHammersley(i);
		vec3 Lh = SampleGGX(u.x, u.y, roughness);
		vec3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

		float cosLi = Li.z;
		float cosLh = Lh.z;
		float cosLoLh = max(dot(Lo, Lh), 0.0);

		if (cosLi > 0.0)
		{
			float G = GaSchlickGGX_IBL(cosLi, cosLo, roughness);
			float Gv = G * cosLoLh / (cosLh * cosLo);
			float Fc = pow(1.0 - cosLoLh, 5.0);

			DFG1 += (1.0 - Fc) * Gv;
			DFG2 += Fc * Gv;
		}
	}

	DFG1 *= InvNumSamples;
	DFG2 *= InvNumSamples;

	FragColor = vec2(DFG1, DFG2);
}