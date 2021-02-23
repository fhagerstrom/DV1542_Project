#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include <iostream>

namespace dx = DirectX;

class Texture
{
private:
	ID3D11Resource* resource;
	ID3D11ShaderResourceView* textureSRV;

public:
	Texture();
	Texture(const Texture&);
	~Texture();

// The first two functions will load a texture from a given file name and unload that texture when it is no longer needed.
	bool Initialize(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

// The GetTexture function returns a pointer to the texture resource so that it can be used for rendering by shaders.
	ID3D11ShaderResourceView* GetTexture();
};