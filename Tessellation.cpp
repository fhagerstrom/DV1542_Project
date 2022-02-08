#include "Tessellation.h"

bool Tessellation::TessellInitialize(ID3D11Device* device)
{
	HRESULT hr;
	std::string shaderData;
	std::ifstream reader;

	// Compile hull shader
	reader.open(hsPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open HS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	hr = device->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &hullShader);
	if (FAILED(hr))
	{
		std::cout << "Failed to create HS shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	// Compile domain shader
	reader.open(dsPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open DS file!" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	hr = device->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &domainShader);

	if (FAILED(hr))
	{
		std::cout << "Failed to create DS shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	// Setup the description of the tessellation constant buffer that is in the hull shader.
	D3D11_BUFFER_DESC tessDesc;

	tessDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessDesc.ByteWidth = sizeof(TessellationBuffer);
	tessDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessDesc.MiscFlags = 0;
	tessDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the hull shader constant buffer from within this class.
	hr = device->CreateBuffer(&tessDesc, NULL, &tessellationBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Tessellation::Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, float tessellAmount)
{

}

