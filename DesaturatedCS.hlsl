/* 

DESATURATED COMPUTE SHADER
This is just for playing around to get a feel for compute shaders.

*/

struct Pixel
{
    int color;
};

StructuredBuffer<Pixel> bufferIn : register(t0);
RWStructuredBuffer<Pixel> BufferOut : register(u0);

float3 readPixel(int x, int y)
{
    float3 output;
    uint index = (x + y * 1024);
    
    output.x = (float) (((bufferIn[index].color) & 0x000000ff)) / 255.0f;
    output.y = (float) (((bufferIn[index].color) & 0x0000ff00) >> 8) / 255.0f;
    output.z = (float) (((bufferIn[index].color) & 0x00ff0000) >> 16) / 255.0f;
    
    return output;
}

void writeToPixel(int x, int y, float3 color)
{
    uint index = (x + y * 1024);
    
    int ired = (int) (clamp(color.r, 0, 1) * 255);
    int igreen = (int) (clamp(color.g, 0, 1) * 255) << 8;
    int iblue = (int) (clamp(color.b, 0, 1) * 255) << 16;
    
    BufferOut[index].color = ired + igreen + iblue;
}

[numthreads(32, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 pixel = readPixel(DTid.x, DTid.y);
    pixel.rgb = pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11;
    writeToPixel(DTid.x, DTid.y, pixel);
}