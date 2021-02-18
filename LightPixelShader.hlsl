Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

// Sampler State
SamplerState SampleTypePoint : register(s0);

// Constant buffers
cbuffer LightBuffer
{
	float3 lightDirection;
	float padding;
};

// Typedefs
struct PixelInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
};

// Pixel Shader
float4 main(PixelInput input) : SV_TARGET
{
	float4 colors;
	float4 normals;
	float3 lightDir; 
	float intensity;
	float4 outputColor;

	// Start by retrieving the color data and normals for this pixel using the point sampler
	// Sample the colors from the color render texture using the point sampler at this texture coordinate location
	colors = colorTexture.Sample(SampleTypePoint, input.tex);

	// Sample the normals from the color render texture using the point sampler at this texture coordinate location
	normals = normalTexture.Sample(SampleTypePoint, input.tex);

	// Can then perform our directional lighting equation using sampled information
	// Invert direction for calculations
	lightDir = -lightDirection;

	// Calculate light intensity
	intensity = saturate(dot(normals.xyz, lightDir));

	// Determine final amount of diffuse color based on current pixel color and light intensity
	outputColor = saturate(colors * intensity);

	return outputColor;

}