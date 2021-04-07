Texture2D Position	: register(t0);
Texture2D NormalMap	: register(t1);
Texture2D Texture	: register(t2);

SamplerState Sampler : register(s0);

cbuffer ExternalData : register(b0)
{
	float3 lightColour;
	float3 lightDir;
}

struct VertexToPixel
{
	float4 position	: SV_POSITION;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	int3 sampleIndices = int3(input.position.xy, 0);

	float3 normal = NormalMap.Load(sampleIndices).xyz;

	float3 position = Position.Load(sampleIndices).xyz;

	float3 diffuse = Texture.Load(sampleIndices).xyz;

	float3 l = -lightDir;

	float lightAmountDL = saturate(dot(normal, l));
	float3 colour = lightColour * lightAmountDL * diffuse;

	return float4(colour, 1.0f);
}