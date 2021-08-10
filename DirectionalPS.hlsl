Texture2D Position : register(t0);
Texture2D NormalMap : register(t1);
Texture2D Texture : register(t2);

cbuffer ExternalData : register(b0)
{
	float3 lightColor;
	float3 lightDir;
}

SamplerState Sampler : register(s0);

struct VertexToPixel
{
	float4 position : SV_POSITION;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	int3 sampleIndices = int3(input.position.xy, 0);

    float3 ambient = { 0.15f, 0.15f, 0.15f };
	float3 normal = NormalMap.Load(sampleIndices).xyz;

	float3 position = Position.Load(sampleIndices).xyz;

	float3 diffuse = Texture.Load(sampleIndices).xyz;

	float3 L = -lightDir;

	float lightAmountDL = saturate(dot(normal, L));
	float3 color = ambient * lightColor * lightAmountDL * diffuse;

	return float4(color, 1.0f);
}