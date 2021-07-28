Texture2D shaderTexture;
SamplerState sampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : color;
};


float4 main(PixelInputType input) : SV_TARGET
{
	float4 texturecolor;
	float4 finalcolor;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	texturecolor = shaderTexture.Sample(sampleType, input.uv);

	// Combine the texture color and the particle color to get the final color result.
	finalcolor = texturecolor * input.color;

	return finalcolor;
}