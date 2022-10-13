cbuffer CBParticles
{
    matrix wvp;
};

struct VertexInput
{ 
    float3 position : POSITION;
};

struct GSInput
{
    float4 position : SV_POSITION;
};

GSInput main(VertexInput input)
{
    GSInput output;
    
    // Calculate the position of the vertex against the world View Projection (WVP)
    output.position = mul(float4(input.position, 1.0f), wvp);
    
	return output;
}