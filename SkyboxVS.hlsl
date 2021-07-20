cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
}

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VertexToPixel
{
	float4 position : POSITION;
	float3 uvw : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	// Create view matrix with no translation.
	matrix viewNoTranslation = view;
	viewNoTranslation._41 = 0;
	viewNoTranslation._42 = 0;
	viewNoTranslation._43 = 0;

	// Output position.
	matrix viewProj = mul(viewNoTranslation, projection);
	output.position = mul(float4(input.position, 1.0f), viewProj);

	// Max depth for polygons.
	output.position.z = output.position.w;

	// Use cubes vertex position as a direction from origin.
	output.uvw = input.position;

	return output;
}