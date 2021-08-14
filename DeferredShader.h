#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class DeferredShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT3 lightPos;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColor;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

public:
	DeferredShaderClass();
	DeferredShaderClass(const DeferredShaderClass&);
	~DeferredShaderClass();

	bool Initialize(ID3D11Device*, HWND);

	void Shutdown();

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4, XMFLOAT4, XMFLOAT3, float, XMFLOAT4, XMFLOAT3, XMFLOAT3);

private:
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);

	void ShutdownShader();

	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4, XMFLOAT4, XMFLOAT3, float, XMFLOAT4, XMFLOAT3, XMFLOAT3);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;

};
