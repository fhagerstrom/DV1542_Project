//cbuffer CBPerParticle
//{
//    matrix wvp;
//    matrix world;
//};

struct Particle
{
    float3 currentPosition : POSITION;
    float3 oldPosition;
};

struct VertexInput
{ 
    uint vertexID : SV_VertexID;
};

struct GSInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

StructuredBuffer<Particle> particleBuffer : register(t0);

GSInput main(VertexInput input)
{
    GSInput output;

    output.position.xyz = particleBuffer[input.vertexID].currentPosition;
    output.position.w = 1.0f;
	
    output.tex = float2(0, 0);
        
	return output;
}