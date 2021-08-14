#pragma once

#include <d3d11.h>
#include <WICTextureLoader.h>
#include <fstream>
using namespace DirectX;

class TextureClass
{

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;

};