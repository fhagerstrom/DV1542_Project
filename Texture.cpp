#include "Texture.h"

Texture::Texture()
{
	texture = 0;
	textureSrv = 0;
}

Texture::~Texture()
{
}


bool Texture::Initialize(ID3D11Device* device, LPCWSTR textureFile)
{
	hr = DirectX::CreateWICTextureFromFile(device, textureFile, nullptr, &textureSrv);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Couldn't create texture from file!", L"TEXTURE ERROR", MB_OK);
		return false;
	}

	return true;
}


void Texture::Shutdown()
{
	// Release the texture view resource.
	if(textureSrv)
	{
		textureSrv->Release();
		textureSrv = 0;
	}

	// Release the texture.
	if(texture)
	{
		texture->Release();
		texture = 0;
	}

	return;
}