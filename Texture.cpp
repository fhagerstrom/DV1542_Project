#include "Texture.h"

Texture::Texture()
{
	resource = nullptr;
	textureSRV = nullptr;
}


Texture::Texture(const Texture& other)
{

}


Texture::~Texture()
{

}

bool Texture::Initialize(ID3D11Device* device, const WCHAR* filename)
{
    HRESULT hr;

	// filename = L"souljaboy.jpg";

	hr = dx::CreateWICTextureFromFile(device, filename, &resource, &textureSRV);

	if (resource != nullptr)
	{
		std::cout << filename << " created successfully!" << std::endl;
	}

	else
	{
		std::cerr << filename << " failed!" << std::endl;
	}

	if (FAILED(hr))
	{
		std::cerr << "Cannot create WIC Texture" << std::endl;
		return false;
	}
	
	return true;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return textureSRV;
}

void Texture::ReleaseTexture()
{
	// Release the texture resource
	if (textureSRV)
	{
		textureSRV->Release();
	}

	return;
}

