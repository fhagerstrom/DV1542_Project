#include "LightShaderClass.h"

LightShader::LightShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
}

LightShader::LightShader(const LightShader&) 
{

}

LightShader::~LightShader()
{

}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFile, WCHAR* psFile)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	return true;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext*, dx::XMFLOAT4X4, dx::XMFLOAT4X4, dx::XMFLOAT4X4, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, dx::XMVECTOR)
{
    return true;
}


