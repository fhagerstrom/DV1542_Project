// External texture-related data.
TextureCube Sky			: register(t0);
SamplerState Sampler	: register(s0);

// Defines the input to this pixel shader.
// - Match with the output of the corresponding vertex shader.
struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 uvw : TEXCOORD;
};

// Entry point.
float4 main(VertexToPixel input) : SV_TARGET
{
	return Sky.Sample(Sampler, input.uvw);
}