#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

namespace dx = DirectX;

class LightShader
{
private:
	struct MatrixBufferType
	{
		dx::XMMATRIX world;
		dx::XMMATRIX view;
		dx::XMMATRIX projection;
	};

	struct LightBufferType
	{
		dx::XMFLOAT3 lightDirection;
		float padding;
	};

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampleStateWrap;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;

	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, dx::XMMATRIX, dx::XMMATRIX, dx::XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, dx::XMFLOAT3);

	void LogLightShaderErrors(ID3DBlob*, HWND, const WCHAR*);
	void RenderShader(ID3D11DeviceContext*, int);

public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, dx::XMMATRIX, dx::XMMATRIX, dx::XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, dx::XMFLOAT3);
	void ReleaseShader();
	// void ReleaseObjects();
};