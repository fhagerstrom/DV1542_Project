#pragma once

#include <DirectXMath.h>
#include <iostream>

#include "DXInit.h"
#include "Vertex.h"

namespace dx = DirectX;

class Water
{
private:
	ID3D11Buffer* vBuffer, *iBuffer;
	ID3D11ShaderResourceView* waterSRV;

	dx::XMFLOAT2 normalMapTiling;

	int vertexCount, indexCount;
	float waterHeight;
	float waterTranslation;

	bool InitializeBuffers(ID3D11Device* device, float waterRadius);
	bool LoadTexture(ID3D11Device* device, const WCHAR* textureFile);
	void RenderBuffers(ID3D11DeviceContext* context);

public:
	Water();
	~Water();

	bool Initialize(ID3D11Device* device, const WCHAR* textureFile, float waterHeight, float waterRadius);
	void Update();
	void Render(ID3D11DeviceContext* context);

	ID3D11ShaderResourceView* GetTexture() { return waterSRV; };

	int GetIndexCount() { return indexCount; };
	float GetWaterHeight() { return waterHeight; }
	float GetSpecularShininess() { return specularShininess; }

	dx::XMFLOAT2 GetNormalMapTiling() { return normalMapTiling; }

};