Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D diffuseTexture : register(t2);

SamplerState SampleTypePoint : register(s0);



cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
	// float3 lightPos;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 lightPos : TEXCOORD1;
    float3 viewDirection : TEXCOOR2;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 colors;
    float4 normals;
    float3 lightDir;
    float lightIntensity;
    float4 outputColor;
    float3 reflection;
    float4 specular;
    float4 diffuse;

    colors = colorTexture.Sample(SampleTypePoint, input.tex);

    normals = normalTexture.Sample(SampleTypePoint, input.tex);

    diffuse = diffuseTexture.Sample(SampleTypePoint, input.tex);

    //outputColor = ambientColor;
    //
    //specular = specularColor;
    //
    //// Invert the light direction for calculations.
    //lightDir = -lightDirection;
    //
    //// Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(normals.xyz, lightDir));
    //
    //if (lightIntensity > 0.0f)
    //{
    //	outputColor += (diffuse * lightIntensity);
    //
    //
    //	reflection = normalize(2 * lightIntensity * normals.xyz - lightDir);
    //	specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    //
    //}
    //	outputColor = outputColor * diffuse;
    //
    //
    //	outputColor = outputColor * colors;
    //
    //
    //	outputColor = saturate(outputColor + specular);
    //
    //// Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    ////outputColor = saturate(colors * lightIntensity);
    //
    ////outputColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
    //if (!colors.x && !diffuse.x)
    //{
    //	outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //}


    return outputColor;
}
