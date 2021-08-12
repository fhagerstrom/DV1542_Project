// External texture-related data
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);
SamplerState Sampler : register(s0);

cbuffer externalData : register(b0)
{
    float3 cameraPosition;
}

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	// Move the position the water normal is sampled from to simulate moving water.	
    input.uv.x = cameraPosition.x;

    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);

	// Normal Mapping
    float3 normalFromMap = NormalMap.Sample(Sampler, input.uv).rgb * 2 - 1;

	// Calculate the TBN matrix to get the normal into world space
    float3 n = input.normal;
    float3 t = normalize(input.tangent - n * dot(input.tangent, n));
    float3 b = cross(t, n);
    float3x3 tbn = float3x3(t, b, n);

    input.normal = normalize(mul(normalFromMap, tbn));

    float4 color = Texture.Sample(Sampler, input.uv);
    
    return color;
}