cbuffer extData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

struct VertexToPixel
{
	float4 position : POSITION;
	float3 worldPos	: POSITION;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix wvp = mul(mul(world, view), projection); // Calculate world view projection matrix.

	output.position = mul(float4(input.position, 1.0f), wvp); // Set position.
	output.normal = mul(input.normal, (float3x3)world); // Set normal equation.
	output.tangent = mul(input.tangent, (float3x3)world); // Set tangent.
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz; // Set world position.
	output.uv = input.uv; // Set texelcoords.

	return output;
}