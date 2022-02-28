cbuffer tessellationBuffer
{
    float tessellAmount;
    float3 padding;
};

// Default name: VS_CONTROL_POINT_INPUT
struct HullInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

// Default name: HS_CONSTANT_DATA_OUTPUT
struct ConstantOutputType
{
	float EdgeTessFactor[3]	: SV_TessFactor;
	float InsideTessFactor  : SV_InsideTessFactor;
};

// Default name: HS_CONTROL_POINT_OUTPUT
struct HullOutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

#define NUM_CONTROL_POINTS 3

ConstantOutputType CalcHSPatchConstants(InputPatch<HullInputType, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
    ConstantOutputType output;
	
	// Set tessellation factors for the edges of object
	// Example: Outside edges, we get 4 vertices per edge if we use a factor of 3. Table for amount can be seen here: https://i.stack.imgur.com/p68tW.png
    output.EdgeTessFactor[0] = tessellAmount;
    output.EdgeTessFactor[1] = tessellAmount;
    output.EdgeTessFactor[2] = tessellAmount;
	
	// Tessellation factor for the inside of object
	output.InsideTessFactor = tessellAmount;

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]

HullOutputType main(InputPatch<HullInputType, NUM_CONTROL_POINTS> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    HullOutputType output;
    
    output.position = ip[i].position;
    output.tex = ip[i].tex;
    output.normal = ip[i].normal;
    output.worldPos = ip[i].worldPos;
    
	return output;
}
