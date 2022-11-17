cbuffer CBPerObject : register(b0)
{
    matrix wvp;
    matrix world;
};

cbuffer cameraBuffer : register(b1)
{
    float3 cameraPos;
    float padding;
};

struct VertexInput
{
    float4 position : POSITION;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

VertexOutput main( VertexInput input )
{
    VertexOutput output;
    
    // Calculate the position of the vertex against the world View Projection (WVP) and world matrix
    output.position = mul(input.position, wvp);
    output.worldPos = mul(input.position, world);
    
    output.tex = float2(0, 0);
    output.normal = float3(0, 0, 0);
    
    output.normal = normalize(output.normal);
    
    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPos.xyz - output.worldPos.xyz;
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}