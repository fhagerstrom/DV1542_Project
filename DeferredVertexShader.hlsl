
// DEFERRED RENDERING___________________________________
cbuffer BufferMatrix
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

PixelInput main(VertexInput input)
{
    PixelInput output;
    
    
    //change pos of vertex to 4 for matrix calc.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projMatrix);
    
 
    output.tex = input.tex;
    
    // Calc. the normal vector vs world matrix 
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize 
    output.normal = normalize(output.normal);

    return output;
}

//_______________________________________________________