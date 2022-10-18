cbuffer CBPerParticle
{
    matrix wvp;
    matrix world;
};

struct VertexInput
{ 
    float3 position : POSITION;
};

struct GSInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
};

GSInput main(VertexInput input)
{
    GSInput output;
    
    // Calculate the position of the vertex against the world View Projection (WVP) and the world matrix
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldPos = mul(float4(input.position, 1.0f), world);
        
	return output;
}