#pragma once

#include "DxHandler.h"

class Texture
{
private:
	ID3D11ShaderResourceView* texture = nullptr;
public:

	ID3D11ShaderResourceView** Get() { return &this->texture; }

	Texture() = default;
	Texture(ID3D11Device* device, std::string path);

	void Bind(ID3D11DeviceContext* context, int slot = 0, shaders shader = shaders::PS)
	{
		switch (shader)
		{
		case shaders::PS:
			context->PSSetShaderResources(slot, 1, &texture);
			break;
		case shaders::DS:
			context->DSSetShaderResources(slot, 1, &texture);
			break;
		}
	}

	~Texture() { this->texture->Release(); }
};

struct Material
{
	struct Data
	{
		XMFLOAT4 diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT4 ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT4 specular = { 0.0f, 0.0f, 0.0f, 1.0f };
	} data;

	std::vector<Texture*> diffuseTextures;

	~Material()
	{
		for (auto& texture : diffuseTextures)
			delete texture;
	}

	Material() = default;
};