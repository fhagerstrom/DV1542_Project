Texture2D shaderTexture;
SamplerState sampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 colour : COLOUR;
};


float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColour;
	float4 finalColour;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColour = shaderTexture.Sample(sampleType, input.uv);

	// Combine the texture color and the particle color to get the final color result.
	finalColour = textureColour * input.colour;

	return finalColour;
}