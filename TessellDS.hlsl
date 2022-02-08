cbuffer constantBuffer
{
    matrix wvp;
};

// Default name: DS_OUTPUT
struct HullOutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

// Default name: HS_CONTROL_POINT_OUTPUT
struct PixelInputType
{
	float4 position : SV_POSITION; 
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

// Default name: HS_CONSTANT_DATA_OUTPUT
struct ConstantOutputType
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
PixelInputType main(ConstantOutputType input, float3 domain : SV_DomainLocation, const OutputPatch<HullOutputType, NUM_CONTROL_POINTS> patch)
{
    float3 vertexPos;
	PixelInputType output;

	// Determine new vertice's position
    vertexPos = domain.x * patch[0].position + domain + domain.y * patch[1].position + domain.z * patch[2].position;
	
	// Calculate position of the new vertex against the wvp matrix
    output.position = mul(float4(vertexPos, 1.0f), wvp);
	
    output.tex = patch[0].tex;
    output.normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.z * patch[2].normal;

	return output;
}
