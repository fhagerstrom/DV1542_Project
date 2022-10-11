cbuffer CBPerObject : register(b0)
{
    matrix wvp;
}

struct VertexInput
{
    float3 position : POSITION;
};

struct PixelInput
{
    float4 wvpPosition : SV_POSITION;
    float3 lightPosition : POSITION;
};


PixelInput main(VertexInput input)
{
    PixelInput output;
    
    // NOTE: WVP position is set to xyww instead of xyzw so that z is always 1.
    // This is to set the skybox / cube map far away so that the camera can never go "past" it.
    
    float4 hPos = mul(wvp, float4(input.position, 1));
    output.wvpPosition = hPos.xyww;
    output.lightPosition = input.position;
    
	return output;
}