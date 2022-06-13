#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <iostream>
#include <string>
#include <vector>

#include "Texture.h"

using namespace DirectX;

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: position(x, y, z), texCoord(u, v), normal(nx, ny, nz) { }

	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

struct Material
{
	std::wstring mtlName;
	XMFLOAT4 diffuseMtl = { 0.0f, 0.0f, 0.0f, 0.0f }; // 4th channel = alpha
	XMFLOAT4 ambientMtl = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT4 specularMtl = { 0.0f, 0.0f, 0.0f, 0.0f }; // Spec power stored in w channel
	XMFLOAT4 reflectionMtl = { 0.0f, 0.0f, 0.0f, 0.0f };

	float shine = 1;

	bool hasTexture = false;
	bool hasReflection = false;
};

class Model
{
public:
	Model();
	Model(std::string name);
	~Model();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);
	bool InitializeCube(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	// GETTERS
	// ---------------------------------------------------------------------------------
	int GetVertexCount() { return this->vertexCount; }
	int GetIndexCount() { return this->indexCount; };
	int& GetSubsetCount() { return this->subsetCount; };

	ID3D11Buffer& GetVertexBuffer() { return *this->vertexBuffer; }
	ID3D11Buffer& GetIndexBuffer() { return *this->indexBuffer; }

	std::vector<Vertex>& GetVertices() { return vertices; }
	std::vector<Material>& GetMaterials() { return materials; }
	std::vector<int>& GetSubsetIndex() { return this->subsetIndexStart; };
	std::vector<int>& GetSubsetMaterial() { return this->subsetMaterial; };

	XMMATRIX GetWorldMatrix() { return this->world; }
	ID3D11ShaderResourceView* GetTexture() { return texture->GetTexture(); };
	// ---------------------------------------------------------------------------------


	// SETTERS
	// ---------------------------------------------------------------------------------
	void SetTexture(ID3D11ShaderResourceView* resource) { this->texture->SetTexture(resource); };
	void SetIndexBuffer(ID3D11Buffer* indexBuffer) { this->indexBuffer = indexBuffer; };
	void SetVertexBuffer(ID3D11Buffer* vertexBuffer) { this->vertexBuffer = vertexBuffer; };
	void SetWorldMatrix(DirectX::XMMATRIX world) { this->world = world; }
	void SetVertexCount(int count) { this->vertexCount = count; }
	void SetIndexCount(int count) { this->indexCount = count; }
	// ---------------------------------------------------------------------------------

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool CreateCube(ID3D11Device* device);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, LPCWSTR);
	void ReleaseTexture();

	HRESULT hr;
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	int vertexCount, indexCount;
	int subsetCount;

	std::string modelName;
	Texture* texture;

	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;
	std::vector<Material> materials;
	std::vector<int> subsetIndexStart;
	std::vector<int> subsetMaterial;

	XMMATRIX world;
};