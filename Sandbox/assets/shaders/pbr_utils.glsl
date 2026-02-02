#pragma once

// GGX Distribution
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

// Geometry Smith
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel Schlick
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Fresnel with roughness (IBL spec)
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//// -----------------------------------------------------------------------------
//// Directional Light Evaluation
//// -----------------------------------------------------------------------------
//vec3 EvaluateDirectionalLight(DirectionalLight light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness)
//{
//    vec3 L = normalize(-light.uLightDir.rgb);
//    vec3 radiance = light.uColor.rgb;
//    return EvaluatePBR(N, V, L, albedo, metallic, roughness) * radiance;
//
//}

//// -----------------------------------------------------------------------------
//// PBR BRDF Evaluation
//// -----------------------------------------------------------------------------
//vec3 EvaluatePBR(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness)
//{
//	vec3 H = normalize(V + L);
//
//	float NdotL = max(dot(N, L), 0.0);
//	float NdotV = max(dot(N, V), 0.0);
//
//	// Base reflectivity
//	vec3 F0 = mix(vec3(0.04), albedo, metallic);
//
//	// Cook-Torrance microfacet BRDF
//	float NDF = DistributionGGX(N, H, roughness);
//	float G = GeometrySmith(N, V, L, roughness);
//	vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);
//
//	vec3 numerator = NDF * G * F;
//	float denominator = 4.0 * NdotV * NdotL + 1e-5;
//	vec3 specular = numerator / denominator;
//
//	// Diffuse term
//	vec3 kS = F;
//	vec3 kD = (1.0 - kS) * (1.0 - metallic);
//	vec3 diffuse = kD * albedo / PI;
//
//	// Combine BRDF
//	return (diffuse + specular) * NdotL;
//}