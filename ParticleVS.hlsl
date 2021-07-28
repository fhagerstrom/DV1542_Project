cbuffer MatrixBuffer : register(b0)
{
	matrix world;
    matrix view;
    matrix projection;
}

struct VertexShaderInput
{
    float4 position : POSITION;
    float2 uv : UVCOORD0;
    float4 color : color;
};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : UVCOORD0;
    float4 color : color;
};


VertexToPixel main(VertexShaderInput input)
{
   VertexToPixel output;

     // Change the pos vector to 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate world, view and proj
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    output.uv = input.uv;

    // Store the particle color for pixel shader. 
    output.color = input.color;

	return output;
}