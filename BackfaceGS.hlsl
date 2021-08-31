struct GSInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 worldPos : WORLDPOS;
	float2 tex : TEXCOORD;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 worldPos : WORLDPOS;
	float2 tex : TEXCOORD;
};

cbuffer CameraPos : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
	float3 camPos;
}

[maxvertexcount(3)]
void main(triangle GSInput input[3] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	float3 p0 = input[0].pos.xyz;
	float3 p1 = input[1].pos.xyz;
	float3 p2 = input[2].pos.xyz;
	//calculate facenormal by using crossproduct from the triangles vertices
	float3 faceNormal = normalize(cross(p1 - p0, p2 - p0));

	// To determine if we are facing the front or back of the triangle we need to do 
	// dot product with facenormal and the negative first vertice of the triangle
	// if that is >= 0 the cull it
	
	//since camera is origin we do this math
	float cullValue = dot(-p0, faceNormal); 

	if (cullValue >= 0.0f)
	{
		for (uint i = 0; i < 3; i++)
		{
			GSOutput element;
			element = input[i];
			output.Append(element);
		}
	}
}