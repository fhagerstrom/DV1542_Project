cbuffer constantBuffer : register(b1)
{
    matrix wvp;
    matrix worldSpace;
};

// Default name: DS_OUTPUT
struct HullOutput
{
	// float4 position : SV_POSITION;
	float3 position : POSITION;
	float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

// Default name: HS_CONTROL_POINT_OUTPUT
struct PixelInput
{
	float4 position : SV_POSITION; 
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

// Default name: HS_CONSTANT_DATA_OUTPUT
struct ConstantOutput
{
	float EdgeTessFactor[3]	: SV_TessFactor;
	float InsideTessFactor	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
PixelInput main(ConstantOutput input, float3 domain : SV_DomainLocation, const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch)
{
    float3 vertexPos;
	PixelInput output;

	// Determine new vertice's position
    // vertexPos = float4(domain.x * patch[0].position + domain + domain.y * patch[1].position + domain.z * patch[2].position, 1);
    vertexPos = domain.x * patch[0].position + domain + domain.y * patch[1].position + domain.z * patch[2].position;
	
	// Calculate position of the new vertex against the world and wvp matrix
    output.position = mul(float4(vertexPos, 1.0f), worldSpace);
    output.position = mul(float4(vertexPos, 1.0f), wvp);
    output.worldPos = output.position;
	
    output.tex = patch[0].tex;
    output.normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.z * patch[2].normal;

	return output;
}
