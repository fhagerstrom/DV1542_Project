// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;

	matrix shadowView;
	matrix shadowProj;
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;

    float4 posForShadow	: POSITION1;
};

// The entry point (main method) for our vertex shader
VertexToPixel main(VertexShaderInput input)
{
	// Set up output
    VertexToPixel output;

	// Calculate output position
    matrix worldViewProj = mul(mul(world, view), projection);
    output.position = mul(float4(input.position, 1.0f), worldViewProj);

    // Calculate the position of this vertex relative to the shadow-casting light
    matrix shadowWVP = mul(mul(world, shadowView), shadowProj);
	output.posForShadow = mul(float4(input.position, 1.0f), shadowWVP);

	// Get the normal to the pixel shader
    output.normal = mul(input.normal, (float3x3) world); // ASSUMING UNIFORM SCALE HERE!!!  If not, use inverse transpose of world matrix
    output.tangent = mul(input.tangent, (float3x3) world);

	// Get world position of vertex
    output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	// Pass through the uv
    output.uv = input.uv;

    return output;
}