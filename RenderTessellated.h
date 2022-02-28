#pragma once
#include "Model.h"
#include "DxHandler.h"
#include "Shaders.h"

class RenderTessellated
{
private:
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;
	std::string byteCode;

	// Use Deferred VS and PS. Use DS and HS for tessellation
	const std::string vsPath = "x64/Debug/DeferredVS.cso";
	const std::string psPath = "x64/Debug/DeferredPS.cso";
	const std::string dsPath = "x64/Debug/TessellDS.cso";
	const std::string hsPath = "x64/Debug/TessellHS.cso";

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11DomainShader* domainShader;
	ID3D11HullShader* hullShader;

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightingBuffer;
	ID3D11Buffer* tessellBuffer;

	MatrixBuffer matrixTessellData;
	TessellationBuffer tessellData;

public:
	RenderTessellated(ID3D11Device* device);
	void Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, float tessellAmount);
	std::string GetByteCode() const { return byteCode; };
	void Shutdown();


};
