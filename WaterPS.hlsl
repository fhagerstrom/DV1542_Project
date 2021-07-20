SamplerState SampleTypePoint : register(s0);
Texture2D reflectionTexture;
Texture2D refractionTexture;
Texture2D normalTexture;

cbuffer WaterBuffer
{
	float waterTranslation;
	float reflectRefractScale;
	float2 padding;
};

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPos : TEXCOORD1;
	float4 refractionPos : TEXCOORD2;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	float2 reflectTexCoord;
	float2 refractTexCoord;
	float4 normalMap;
	float3 normal;
	float4 reflectColor;
	float4 refractionColor;
	float4 color;

	// Move position that we sampled the water normal from. This will simulate moving water.
	input.tex.y += waterTranslation;

	// Calculate projected reflection texture coords
	reflectTexCoord.x = input.reflectionPos.x / input.reflectionPos.w / 2.0f + 0.5f;
	reflectTexCoord.y = input.reflectionPos.y / input.reflectionPos.w / 2.0f + 0.5f;

	// Calculate the projected refraction texture coords
	refractTexCoord.x = input.refractionPos.x / input.refractionPos.w / 2.0f + 0.5f;
	refractTexCoord.y = input.refractionPos.y / input.refractionPos.w / 2.0f + 0.5f;

	// Sample normal from normal map texture
	normalMap = normalTexture.Sample(SampleTypePoint, input.tex);

	// Expand range of normal from (0, 1) to (-1, +1)
	normal = (normalMap.xyz * 2.0f) - 1.0f;

	// Re-position the texture coord sampling pos by normal map value to simulate rippling wave effect
	reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
	refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);

	// Sample texture pixels from textures using updated texture coords
	reflectColor = reflectionTexture.Sample(SampleTypePoint, reflectTexCoord);
	refractionColor = refractionTexture.Sample(SampleTypePoint, refractTexCoord);

	// Combine reflection AND refraction results to get the final color
	color = lerp(reflectColor, refractionColor, 0.6f);

	return color;
}