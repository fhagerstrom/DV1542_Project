#pragma once

#include <d3d11.h>
#include <directxmath.h>

#include "Texture.h"

using namespace DirectX;

struct Vertex
{
	Vertex() : position(), texCoord(), normal() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: position(x, y, z), texCoord(u, v), normal(nx, ny, nz) { }

	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

class Model
{
public:
	Model();
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, LPCWSTR);
	bool InitializeCube(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool CreateCube(ID3D11Device* device);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, LPCWSTR);
	void ReleaseTexture();

private:
	HRESULT hr;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;
	Vertex* modelData;
	Texture* texture;
};