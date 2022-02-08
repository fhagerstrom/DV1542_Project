#pragma once
#include "Defines.h"
#include "DxHandler.h"
#include "Model.h"
#include "RenderModels.h"

class Tessellation
{
private:
	struct TessellationBuffer
	{
		float tessellAmount;
		XMFLOAT3 padding;
	};

	const unsigned int stride = sizeof(XMFLOAT3);
	const unsigned int offset = 0;

	// Using regular deferred VS and PS. Also using a DS and HS
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11DomainShader* domainShader;
	ID3D11HullShader* hullShader;

	const std::string vsPath = "x64/Debug/DeferredVS.cso";
	const std::string psPath = "x64/Debug/DeferredPS.cso";
	const std::string dsPath = "x64/Debug/TessellDS.cso";
	const std::string hsPath = "x64/Debug/TessellHS.cso";

	ID3D11Buffer* tessellationBuffer;

public:
	Tessellation() = default;
	bool TessellInitialize(ID3D11Device* device);
	void Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, float tessellAmount);
};