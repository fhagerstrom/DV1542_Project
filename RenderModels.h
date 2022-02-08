#pragma once
#pragma once
#include "Model.h"
#include "DxHandler.h"
#include "Shaders.h"

struct Matrix
{
	XMFLOAT4X4 wvp;
	XMFLOAT4X4 worldSpace;
};

// For tessellation
struct TessellationBuffer
{
	float tessellAmount;
	XMFLOAT3 padding;
};

class RenderModels
{
private:
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;
	std::string byteCode;

	// Deferred
	const std::string vs_path = "x64/Debug/DeferredVS.cso";
	const std::string ps_path = "x64/Debug/DeferredPS.cso";
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	// Tessellation
	const std::string dsPath = "x64/Debug/TessellDS.cso";
	const std::string hsPath = "x64/Debug/TessellHS.cso";
	ID3D11DomainShader* domainShader;
	ID3D11HullShader* hullShader;

	ID3D11Buffer* lightingBuffer;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessellBuffer;

	Matrix matrixData;
	TessellationBuffer tessellData;


public:
	RenderModels(ID3D11Device* device);
	void Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, bool rotation, float tessellAmount);
	std::string GetByteCode() const;
	void ShutDown();
	int modelsRendered;
};