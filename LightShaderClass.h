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
		dx::XMVECTOR lightDirection;
		float padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, dx::XMMATRIX, dx::XMMATRIX, dx::XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, dx::XMVECTOR);

	void ReleaseShader();
	void LogShaderErrors(ID3DBlob*, HWND, WCHAR*);
	void RenderShader(ID3D11DeviceContext*, int);

public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, dx::XMFLOAT4X4, dx::XMFLOAT4X4, dx::XMFLOAT4X4, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, dx::XMVECTOR);

	void ReleaseObjects();
};