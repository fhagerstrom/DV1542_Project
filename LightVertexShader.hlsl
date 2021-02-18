cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

//no need for normal in struct since deferred means 2d post processing

struct VertexInput
{
	float4 position : SV_POSITION;
	float2 tex: TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex: TEXCOORD;
};

// vs will forward the pos and texture coords of 2d image

VertexInput main(VertexInput input) 
{
	PixelInput output;

	//make pos vector 4 units to do matrix calc
	input.position.w = 1.0f;

	//calc pos of vertex against world. view and proj
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	return output;
}