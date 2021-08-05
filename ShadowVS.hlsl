cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
	matrix lightView;
	matrix lightProj;
};

cbuffer lightBuffer
{
	float3 lightPos;
	float padding;
};

struct VertexToPixel
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelShaderOutput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

PixelShaderOutput main(VertexToPixel input)
{
	PixelShaderOutput output;
	float4 worldPos;

	//change position of vector to 4 units for 
	//correct matrix calculations
	input.position.w = 1.0f;

	//calculate position of vector against world view proj
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, view);

	//transform matrix based on the perspective of the light 
	
	output.lightViewPosition = mul(input.position, world);
	output.lightViewPosition = mul(output.lightViewPosition, lightView);
	output.lightViewPosition = mul(output.lightViewPosition, lightProj);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)world);

	output.normal = normalize(output.normal);

	worldPos = mul(input.position, world);

	output.lightPos = lightPos.xyz - worldPos.xyz;

	output.lightPos = normalize(output.lightPos);


	return output;
}



