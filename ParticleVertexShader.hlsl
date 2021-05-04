cbuffer MatrixBuffer
{
	matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

struct VertexInputType
{
    float4 position : POSITION;
    float2 uv : uvCOORD0;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : uvCOORD0;
    float4 color : COLOR;
};


PixelInputType main(VertexInputType input)
{
    PixelInputType output;

     // Change the pos vector to 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate world, view and proj
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    

    output.uv = input.uv;

    // Store the particle color for pixel shader. 
    output.color = input.color;

	return output;
}