cbuffer MatrixBufferType
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
    float3 campos;
    float waterTime;
};

struct GSInput
{

    float4 Pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};


struct GSOutput
{
    float4 Pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

[maxvertexcount(64)] // Update
void main(triangle GSInput input[3], inout TriangleStream<GSInput> OutputStream)
{


    for (uint i = 0; i < 3; i++)
    {

        GSOutput output;
		// = (GSOutput)0;

		//water_____________________________________________

        float component1 = sin(2.0 * 3.14 * waterTime + (input[i].Pos.x * 16.0)) * 0.01;
        float component2 = sin(2.0 * 3.14 * waterTime + (input[i].Pos.z * input[i].Pos.x * 8.0));
        float height = component1 + component2;
        output.Pos = float4(input[i].Pos.x, height / 3, input[i].Pos.z, 1.0f);

		//______________________
        float3 u = input[1].Pos - input[0].Pos;
        float3 v = input[2].Pos - input[0].Pos;
        float3 norm = cross(u, v);


        float4 offset = float4(norm, 0.0f) + input[i].Pos;

        output.Pos = mul(output.Pos, world);
        output.Pos = mul(output.Pos, view);
        output.Pos = mul(output.Pos, projection);

        float3 camToTriangle = normalize(input[i].Pos - campos);

        output.worldPos = mul(input[i].worldPos, world);
        output.uv = input[i].uv;
        output.normal = mul(norm, world);

        if (dot(camToTriangle, norm) <= 0) 
            OutputStream.Append(output);
    }

}