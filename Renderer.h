#pragma once

#include "EngineEntity.h"
#include "Camera.h"
#include "Lights.h"
#include "Water.h"

class Renderer
{
private:
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	DirectionalLight dirLight_1;
	AmbientLight ambientLight;
	PointLight pointLight1;
	PointLight pointLight2;
	PointLight pointLight3;
	PointLight pointLight4;

	PointLight outerPointLight1;
	PointLight outerPointLight2;
	PointLight outerPointLight3;
	PointLight outerPointLight4;
	PointLight outerPointLight5;
	PointLight outerPointLight6;
	PointLight outerPointLight7;
	PointLight outerPointLight8;

	SpotLight spotLight;

	Water* water;

	void SetLights();
public:
	Renderer();
	~Renderer();

	void RenderProcess(EngineEntity*& entity, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, SimpleVertexShader*& vertexShader, SimplePixelShader*& pixelShader, 
		Camera*& camera, ID3D11DeviceContext*& context);
	void RenderGBuffer(EngineEntity*& entity, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, SimpleVertexShader*& vertexShader, SimplePixelShader*& pixelShader, 
		Camera*& camera, ID3D11DeviceContext*& context);
	void RenderWater(EngineEntity*& entity, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, SimpleVertexShader*& vertexShader, SimplePixelShader*& pixelShader,
		Camera*& camera, ID3D11DeviceContext*& context);
	void RenderSkyBox(Mesh*& skyMesh, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, SimpleVertexShader*& vertexShader, SimplePixelShader*& pixelShader,
		Camera*& camera, ID3D11DeviceContext*& context, ID3D11ShaderResourceView*& skySRV, ID3D11RasterizerState*& skyRasterizerState, ID3D11DepthStencilState*& skyDepthState);
	void RenderLights(EngineEntity*& entity, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, SimpleVertexShader*& vertexShader, SimplePixelShader*& pixelShader, 
		Camera*& camera, ID3D11DeviceContext*& context, ID3D11SamplerState*& sampler, ID3D11ShaderResourceView*& positionGBuffer, ID3D11ShaderResourceView*& normalGBuffer, ID3D11ShaderResourceView*& diffuseGBuffer);
};

