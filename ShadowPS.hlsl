Texture2D shaderTexture : register(t0);

Texture2D depthMapTexture : register(t1);

SamplerState SampleClamp : register(s0);
SamplerState SampleWrap : register(s1);

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
};

struct PixelShaderOutput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

float4 main(PixelShaderOutput input) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	// Shadow mapping requeires a bias adjustment when comparing
	// Depth of light and depth of object because of low floating
	// Point precision of depth map.

	color = ambientColor;
	bias = 0.001f;


	projTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	//check if proj coords are in the view of light, if not ouxel gets abient value
	if ((saturate(projTexCoord.x) == projTexCoord.x) && (saturate(projTexCoord.y == projTexCoord.y)))
	{
		//here we are in the view of light, so we retreive depth value from shadow map.
		//we sample only the red component since this is gray scale texture- 
		// depth value we get from texture translates into distance to nearest object.
		//this is important since object are what cast shadows, hence why it is called shadow map

		depthValue = depthMapTexture.Sample(SampleClamp, projTexCoord).r;

		// After we have depth of object we need depth of light to determine if it is in front
		// of or behind the object. We get this from lightViewPosition
		//we substract bias so we dont get point precision issues
		float t = 1;
		float y = 2;
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
		lightDepthValue = lightDepthValue - bias;


		// here we compare depth of shadow map value with depth of light to
		//determine if we do shadow or light to the pixel in question
		//if light is in front of object, then we light the pixel.
		//if not we shadow the pixel
		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos));
			if (lightIntensity > 0.0f);
			{
				//determine final diffuse color
				color += (diffuseColor * lightIntensity);

				//saturate final light color
				color = saturate(color);
			}
		}
	}
	//sample pixel color from texxture using the sampler at this texture coord location
	textureColor = shaderTexture.Sample(SampleWrap, input.tex);
	
	//combine light and texture color
	color = color * textureColor;

	return color;
		 
}