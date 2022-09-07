cbuffer CBPerObject
{
	row_major matrix worldMatrix;
	//matrix viewMatrix;
	//matrix projectionMatrix;
    row_major matrix wvp;
};

cbuffer cameraBuffer
{
    float3 cameraPos;
    float padding;
};

struct VertexInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

	// Calculate the position of the vertex against the world View Projection (WVP) and world matrix
    // output.position = mul(input.position, worldMatrix);
    //output.position = mul(output.position, viewMatrix);
    //output.position = mul(output.position, projectionMatrix);
    
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    
    // Same goes for normals.
    output.normal = normalize(output.normal);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPos.xyz - output.worldPos.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}