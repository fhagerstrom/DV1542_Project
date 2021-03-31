Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float2 uv : TEXELCOORD0;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 totalColour = Texture.Sample(Sampler, input.uv);
	return totalColour;
}