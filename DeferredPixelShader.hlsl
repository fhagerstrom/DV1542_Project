Texture2D Texture : register(t0);
Texture2D NormalMap : register (t1);
SamplerState Sampler : register(s0);

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 worldPos		: POSITIONWS;
	float3 normal		: NORMALWS;
	float3 tangent		: TANGENTWS;
	float2 uv			: TEXCOORD;
};

struct PixelShaderOutput
{
	float4 normMap : SV_TARGET0; // Normal map
	float4 diffuse : SV_TARGET1; // Colour
	float4 position : SV_TARGET2; // Position
};

PixelShaderOutput main(VertexToPixel input)
{
	PixelShaderOutput output;

	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Read normal from mapping
	float3 normalFromMap = NormalMap.Sample(Sampler, input.uv).xyz * 2 - 1;

	// Tangent to world space
	float3 n = input.normal;
	float3 t = normalize(input.tangent - n * dot(input.tangent, n));
	float3 b = cross(t, n);

	float3x3 tbn = float3x3(t, b, n);
	input.normal = normalize(mul(normalFromMap, tbn));

	float4 textureColour = Texture.Sample(Sampler, input.uv);

	output.normMap = float4(input.normal, 1.0f);
	output.diffuse = textureColour;
	output.position = float4(input.worldPos, 1.0f);

	return output;
}