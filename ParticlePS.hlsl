struct GSOutput
{
    float4 pos : SV_POSITION;
};

struct GeoemtryToPixel
{
    float4 position : SV_TARGET0;
    float4 diffuse : SV_TARGET1;
};

GeoemtryToPixel main(GSOutput input)
{
    GeoemtryToPixel output;
    
    output.position = input.pos;
    output.diffuse = float4(68.0f / 255.0f, 85.0f / 255.0f, 90.0f / 255.0f, 1.0f);
    
    return output;
}