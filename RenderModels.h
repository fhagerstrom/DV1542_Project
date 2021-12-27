#pragma once
#pragma once
#include "Model.h"
#include "DirectXHelper.h"
#include "Shaders.h"

class RenderModels
{
private:
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;
	std::string byteCode;

	//Deferred
	const std::string ps_path = "x64/Debug/DeferredPS.cso";
	ID3D11PixelShader* pixelShader;
	const std::string vs_path = "x64/Debug/DeferredVS.cso";
	ID3D11VertexShader* vertexShader;

	ID3D11Buffer* lightingBuffer;
	ID3D11Buffer* matrixBuffer;

	struct Matrix
	{
		XMFLOAT4X4 wvp;
		XMFLOAT4X4 worldSpace;
	}matrix;

public:

	RenderModels(ID3D11Device* device);

	void Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, bool rotation);

	std::string GetByteCode() const { return this->byteCode; }

	void ShutDown();
};