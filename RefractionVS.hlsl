cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

cbuffer clipPlaneBuffer
{
	float4 clipPlane;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};


struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
	float clip : SV_ClipDistance0;
};


VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	// Calculate output position.
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	
	// Get normal to the pixel shader.
	output.normal = mul(input.normal, (float3x3)world); // Uniform scale. If not, inverse transpose world matrix.
	output.tangent = mul(input.tangent, (float3x3)world); // For normal mapping.

	// Get world matrix of vertex.
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	// Pass to the texelcoordinates.
	output.uv = input.uv;

	output.clip = dot(mul(input.position, world), clipPlane);

	return output;
}