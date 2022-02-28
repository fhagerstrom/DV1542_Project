#pragma once
#include "Shaders.h"
#include "DxHandler.h"
#include "Model.h"

class RenderShadows
{
private:
	const unsigned int stride = sizeof(XMFLOAT3);
	const unsigned int offset = 0;

	// Only using the vertexshader
	const std::string vsPath = "x64/Debug/ShadowVS.cso";
	ID3D11VertexShader* vertexShader;

	ID3D11Buffer* matrixBuffer;
public:

	RenderShadows(ID3D11Device* device);
	void Render(ID3D11DeviceContext* context, Model* model);
	void ShutDown();
};