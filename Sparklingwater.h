#pragma once
#include "DXInit.h"
#include "Vertex.h"
#include <DirectXMath.h>

namespace dx = DirectX;

class Water
{
private:
	ID3D11Buffer* vBuffer, *iBuffer;
	ID3D11ShaderResourceView* waterSRV;

	dx::XMFLOAT2 normalMapTiling;
	dx::XMFLOAT4 refractionTint;

	int vertexCount, indexCount;
	float waterHeight;
	float waterTranslation;
	float reflectRefractScale;
	float specularShininess;

	bool InitializeBuffers(ID3D11Device* device, float waterRadius);
	bool LoadTexture(ID3D11Device* device, const WCHAR* textureFile);
	void RenderBuffers(ID3D11DeviceContext* context);

public:
	Water();
	~Water();

	bool Initialize(ID3D11Device* device, const WCHAR* textureFile, float waterHeight, float waterRadius);
	void Update();
	void Render(ID3D11DeviceContext* context);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	float GetWaterHeight() { return waterHeight; }
	float GetWaterTranslation() { return waterTranslation; }
	float GetReflectRefractScale() { return reflectRefractScale; }
	float GetSpecularShininess() { return specularShininess; }

	dx::XMFLOAT2 GetNormalMapTiling() { return normalMapTiling; }
	dx::XMFLOAT4 GetRefractionTint() { return refractionTint; }

};