#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(std140, binding = 0) uniform Camera
{
	mat4 uView;
	mat4 uProjection;
	mat4 uViewProjection;
	vec4 uCameraPosition;
};

uniform mat4 uModel;
uniform mat4 uSceneRotation; // currently rotated model and scene

out VertexOutput
{
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
} vs_out;

void main()
{
	mat4 modelRotated = uSceneRotation * uModel;

	vec4 worldPos = modelRotated * vec4(aPosition, 1.0);
	vs_out.FragPos = worldPos.xyz;

	//flip texture
	//vs_out.TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
	vs_out.TexCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(modelRotated)));

	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	vs_out.TBN = mat3(T, B, N);

	gl_Position = uViewProjection * worldPos;
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require
#include "common.glsl"
#include "pbr_utils.glsl"

layout(location = 0) out vec4 oFragColor;

in VertexOutput
{
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
} fs_in;

uniform bool bModelType;

layout(binding = 0) uniform sampler2D uAlbedoMap;
layout(binding = 1) uniform sampler2D uNormalMap;
layout(binding = 2) uniform sampler2D uMetallicRoughnessMap;
layout(binding = 3) uniform sampler2D uRoughnessMap;
layout(binding = 4) uniform sampler2D uAOMap;
layout(binding = 5) uniform sampler2D uEmissiveMap;

//enviroment
layout(binding = 6) uniform samplerCube uIrradianceMap;//6
layout(binding = 7) uniform samplerCube uRadianceMap;//7
layout(binding = 8) uniform sampler2D uBRDFLutMap;//8


vec3 GetNormalFromMap(vec2 texCoord, mat3 TBN, sampler2D normalMap)//standard 3 channel
{
	vec3 tangentNormal = texture(normalMap, texCoord).rgb;
	tangentNormal = tangentNormal * 2.0 - 1.0; // [0,1] to [-1,1]
	return normalize(TBN * tangentNormal); // Tangent to world space
}

struct MaterialData
{
	sampler2D metallicRoughnessTextureSampler;
	uint metallicRoughnessTextureUV;
};

MaterialData GetMaterial()
{
	MaterialData retMat;
	retMat.metallicRoughnessTextureSampler = uRoughnessMap;
	retMat.metallicRoughnessTextureUV = 0;
	return retMat;
}

vec4 sampleMetallicRoughness(vec2 tc, MaterialData mat) {
	return texture(mat.metallicRoughnessTextureSampler, tc);
}

struct PBRInfo {
	// geometry properties
	float NdotL;                  // cos angle between normal and light direction
	float NdotV;                  // cos angle between normal and view direction
	float NdotH;                  // cos angle between normal and half vector
	float LdotH;                  // cos angle between light direction and half vector
	float VdotH;                  // cos angle between view direction and half vector
	vec3 n;                       // normal at surface point
	vec3 v;                       // vector from surface point to camera

	// material properties
	float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
	vec3 reflectance0;            // full reflectance color (normal incidence angle)
	vec3 reflectance90;           // reflectance color at grazing angle
	float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
	vec3 baseDiffuseColor;            // color contribution from diffuse lighting
	vec3 baseSpecularColor;           // color contribution from specular lighting

	vec3 FssEss;
	float brdf_scale;
	float brdf_bias;
};

// -----------------------------------------------------------------------------
// PBR BRDF Evaluation (Direct lighting)
// -----------------------------------------------------------------------------
vec3 EvaluatePBR(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness)
{
	vec3 H = normalize(V + L);

	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);

	// Base reflectivity
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	// Cook-Torrance microfacet BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * NdotV * NdotL + 1e-5;
	vec3 specular = numerator / denominator;

	// Diffuse term
	vec3 kS = F;
	vec3 kD = (1.0 - kS) * (1.0 - metallic);
	vec3 diffuse = kD * albedo / PI;

	// Combine BRDF
	return (diffuse + specular) * NdotL;
}

// -----------------------------------------------------------------------------
// Directional Light
// -----------------------------------------------------------------------------
vec3 EvaluateDirectionalLight(DirectionalLight light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness)
{
	vec3 L = normalize(-light.uLightDir.xyz);
	vec3 radiance = light.uColor.rgb;
	return EvaluatePBR(N, V, L, albedo, metallic, roughness) * radiance;
}

// -----------------------------------------------------------------------------
// Image-Based Lighting (IBL)
// -----------------------------------------------------------------------------
vec3 irradiance;
vec3 prefilteredColor;
vec2 brdf;
vec3 EvaluateIBL(vec3 N, vec3 V, vec3 albedo, float metallic, float roughness)
{
	vec3 R = reflect(-V, N);
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	// Fresnel term with roughness
	float NdotV = max(dot(N, V), 0.0);
	vec3 F = FresnelSchlickRoughness(NdotV, F0, roughness);

	// Sample diffuse irradiance
	irradiance = texture(uIrradianceMap, N).rgb;
	vec3 diffuseIBL = texture(uIrradianceMap, N).rgb * albedo;

	// Sample prefiltered environment map for specular IBL
	//const float MAX_REFLECTION_LOD = 5.0;
	int specularTextureLevels = textureQueryLevels(uRadianceMap);
	prefilteredColor = textureLod(uRadianceMap, R, roughness * specularTextureLevels).rgb;
	brdf = texture(uBRDFLutMap, vec2(NdotV, roughness)).rg;
	vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 kS = F;
	vec3 kD = (1.0 - kS) * (1.0 - metallic);

	return kD * diffuseIBL + specularIBL;
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
void main()
{
	vec3 albedo = sRGBtoLinear(texture(uAlbedoMap, fs_in.TexCoords)).rgb;
	//vec3 normal = GetNormalFromMap(fs_in.TexCoords, fs_in.TBN, uNormalMap);

	vec3 normal = normalize(2.0 * texture(uNormalMap, fs_in.TexCoords).rgb - 1.0);
	normal = normalize(fs_in.TBN * normal);

	vec3 mr = texture(uMetallicRoughnessMap, fs_in.TexCoords).rgb;
	vec3 rough = texture(uRoughnessMap, fs_in.TexCoords).rgb;

	float metallic;
	float roughness;
	float ao;

	if(!bModelType)
	{
		metallic = mr.b;
		roughness = max(mr.g, 0.05);
		ao = texture(uAOMap, fs_in.TexCoords).r;
	}
	else
	{
		metallic = mr.r;
		roughness = max(rough.r, 0.05);
		ao = texture(uAOMap, fs_in.TexCoords).r;
	}

	vec3 V = normalize(uCameraPosition.xyz - fs_in.FragPos);

	// Direct Light
	vec3 directLight = EvaluateDirectionalLight(uDirectionalLight, normal, V, albedo, metallic, roughness);

	// Environment IBL
	vec3 ibl = EvaluateIBL(normal, V, albedo, metallic, roughness);

	// Combine
	vec3 color = directLight + ibl;
	if(ao > 0.0f)
	color *= ao;

	 //Emissive
	vec3 emissive = texture(uEmissiveMap, fs_in.TexCoords).rgb;
	//if(emissive > 0.0)
	color += emissive;

	oFragColor = vec4(color, 1.0);


	//debug
	//oFragColor = vec4(albedo, 1.0);
	//oFragColor = vec4(normal * 0.5 + 0.5, 1.0);
	///oFragColor = vec4(vec3(metallic), 1.0);
	//oFragColor = vec4(vec3(roughness), 1.0);
	//oFragColor = vec4(vec3(r.g), 1.0);

	//oFragColor = vec4(irradiance, 1.0);
	//oFragColor = vec4(prefilteredColor, 1.0);
	//oFragColor = vec4(vec3(brdf.x, brdf.y, 0.0), 1.0);
}