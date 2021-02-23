#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <string>
#include <iostream>

namespace dx = DirectX;

class DeferredShader
{
private:
	struct MatrixBuffer
	{
		dx::XMMATRIX view;
		dx::XMMATRIX world;
		dx::XMMATRIX projection;
	};

	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, dx::XMMATRIX, dx::XMMATRIX, dx::XMMATRIX, ID3D11ShaderResourceView*);

	void RenderShader(ID3D11DeviceContext* immediateContext, int indexCount);
	
	void LogDeferredShaderErrors(ID3DBlob*, HWND, const WCHAR*);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampleStateWrap;
	ID3D11Buffer* matrixBuffer;


public:
	DeferredShader();
	DeferredShader(const DeferredShader&);
	~DeferredShader();

	void ReleaseShader();
	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, dx::XMMATRIX, dx::XMMATRIX, dx::XMMATRIX, ID3D11ShaderResourceView*);
	// void ReleaseObjects();
	
};

