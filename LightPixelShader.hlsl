Texture2D Texture : register(t0);
Texture2D NormalMap	: register(t1);
Texture2D PositionTexture : register(t2);

SamplerState basicSampler : register(s0);

cbuffer LightData : register(b0)
{
	float3 cameraPosition;
	float3 lightColour;
	float3 lightPosition;
}

struct VertexToPixel
{
	float4 position	: SV_POSITION;
};

float4 main(in VertexToPixel input) : SV_TARGET0
{
	int3 sampleIndices = int3(input.position.xy, 0);

	float3 normal = NormalMap.Load(sampleIndices).xyz;
	float3 position = PositionTexture.Load(sampleIndices).xyz;
	float3 diffuse = Texture.Load(sampleIndices).xyz;

	float3 l = lightPosition - position;
	float distance = length(l);

	if (distance > 3.0f)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	l /= distance;

	float attenuation = max(0.0f, 1.0f - (distance / 3.0f));

	float lightAmount = saturate(dot(normal, l));
	float3 colour = lightAmount * lightColour * attenuation;

	// Specular calculation.
	float3 viewVector = cameraPosition - position;
	float3 h = normalize(l + viewVector);
	float specular = pow(saturate(dot(normal, h)), 10) * attenuation;

	float3 finalDiffuse = colour * diffuse;
	float3 finalSpecular = specular * diffuse * attenuation;

	float4 totalColour = float4(finalDiffuse + finalSpecular, 1.0f);
	return totalColour;
}