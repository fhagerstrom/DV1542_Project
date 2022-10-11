cbuffer MatrixBuffer : register(b0)
{
    matrix wvp;
    matrix world;
}

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

PixelInput main(VertexInput input)
{
    PixelInput output = (PixelInput)0;
    
    input.position.w = 1.0f;
    output.position = mul(wvp, float4(input.position));
    output.worldPosition = mul(world, float4(input.position));
    output.uv = input.uv;
    
    output.color = input.color;
    
	return output;
}