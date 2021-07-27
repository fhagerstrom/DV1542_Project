cbuffer externalData : register(b0)
{
	float tessellationAmount;
}


 //input control point
struct VertexToHull
{
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

//output control point
struct HullToDomain
{
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VertexToHull, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	Output.EdgeTessFactor[0] = tessellationAmount;
	Output.EdgeTessFactor[1] = tessellationAmount;
	Output.EdgeTessFactor[2] = tessellationAmount;
	Output.InsideTessFactor = tessellationAmount;

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HullToDomain main( 
	InputPatch<VertexToHull, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HullToDomain Output;
	//  code to compute Output here

	Output.normal = ip[i].normal;
	Output.tangent = ip[i].tangent;
	Output.worldPos = ip[i].worldPos;
	Output.uv = ip[i].uv;

	return Output;
}
