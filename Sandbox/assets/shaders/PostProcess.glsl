#type vertex
#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;

out vec2 vTexCoords; 

void main()
{
	vTexCoords = aTexCoords;
	gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 oFragColor;
// Tone-mapping & gamma correction.
const float gamma = 2.2;
const float exposure = 1.0;
const float pureWhite = 1.0;

in vec2 vTexCoords;
uniform vec2 uResolution;
uniform sampler2D uScreenTexture;


vec3 ToneMapping(vec3 color)
{
	float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);

	// Scale color by ratio of average luminances.
	vec3 mappedColor = (mappedLuminance / luminance) * color;

	return color = mappedColor;

}

// Simple hash function to generate pseudo-random float from integer
float hash(int x, int y) {
	int h = x * 374761393 + y * 668265263; // large primes
	h = (h ^ (h >> 13)) * 1274126177;
	return float(h & 0xFFFFFF) / float(0xFFFFFF); // normalize to [0,1]
}

vec3 PBRNeutralToneMapping( vec3 color ) {
  const float startCompression = 0.8 - 0.04;
  const float desaturation = 0.15;

  float x = min(color.r, min(color.g, color.b));
  float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
  color -= offset;

  float peak = max(color.r, max(color.g, color.b));
  if (peak < startCompression) return color;

  const float d = 1. - startCompression;
  float newPeak = 1. - d * d / (peak + d - startCompression);
  color *= newPeak / peak;

  float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
  return mix(color, newPeak * vec3(1, 1, 1), g);
}

vec4 DrawPixels(ivec2 pixel)
{
	// Checkerboard pattern
	int checkerX = pixel.x / 8;
	int checkerY = pixel.y / 8;

	// Generate color for this checker
	vec3 color;
	color.r = hash(checkerX, checkerY);
	color.g = hash(checkerX + 1, checkerY + 1); // offset for different channel
	color.b = hash(checkerX + 2, checkerY + 2);

	return vec4(color, 1.0);
}

void main()
{

	// Get integer pixel coordinates
	//ivec2 pixel = ivec2(gl_FragCoord.xy);
	//oFragColor = DrawPixels(pixel);

	vec3 color = texture(uScreenTexture, vTexCoords).rgb * exposure;
	
	//tonemap
	//color = ToneMapping(color);
	color = PBRNeutralToneMapping(color);
	// Gamma correction.
	color = pow(color, vec3(1.0 / gamma));
	oFragColor = vec4(color,1.0);
	
}
