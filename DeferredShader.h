#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <string>
#include <iostream>

namespace dx = DirectX;

class DeferredShader
{
private:
	struct MatrixBuffer
	{
		dx::XMFLOAT4X4 world;
		dx::XMFLOAT4X4 view;
		dx::XMFLOAT4X4 proj;
	};
	bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode);
	bool InitializeShader(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode);
	void ShutdownShader();
	

	bool SetShaderParameters(ID3D11DeviceContext* immediateContext, dx::XMFLOAT4X4 world, dx::XMFLOAT4X4 view, dx::XMFLOAT4X4 proj, ID3D11ShaderResourceView* textureSRV);
	void RenderShader(ID3D11DeviceContext* immediateContext, int indexCount);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampleStateWrap;
	ID3D11Buffer* matrixBuffer;


public:
	DeferredShader();
	DeferredShader(const DeferredShader&);
	~DeferredShader();



	bool Render(ID3D11DeviceContext*, int indexCount, dx::XMFLOAT4X4 world, dx::XMFLOAT4X4 view, dx::XMFLOAT4X4 proj, ID3D11ShaderResourceView* textureSRV);

	
};

