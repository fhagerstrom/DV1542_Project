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

	int textureArrayIndex = 0;

	bool hasTexture = false;
	bool hasReflection = false;
	bool hasNormalMap = false;

	bool isTransparent = false;

	XMFLOAT4 translation = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Model
{
public:
	Model();
	Model(std::string name);
	~Model();

	bool InitializeCube(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);
	bool InitializeQuad(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);

	bool LoadNormalMap(ID3D11Device*, LPCWSTR);
	void LoadTextureObj(ID3D11ShaderResourceView* textureSrv);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	// GETTERS
	// ---------------------------------------------------------------------------------
	int GetVertexCount() { return this->vertexCount; }
	int GetIndexCount() { return this->indexCount; };
	int& GetSubsetCount() { return this->subsetCount; };
	std::vector<int>& GetSubsetIndexVector() { return subsetIndexStart; }

	std::vector<Vertex>& GetVertices() { return vertices; }
	std::vector<DWORD>& GetIndices() { return indices; }
	std::vector<XMFLOAT3>& GetVerticesArray() { return vertexPosArray; }
	std::vector<Material>& GetMaterials() { return this->materials; }
	std::vector<int>& GetSubsetIndex() { return this->subsetIndexStart; }
	std::vector<int>& GetSubsetMaterialVector() { return this->subsetMaterial; }

	ID3D11ShaderResourceView* GetTexture() { return texture->GetTexture(); }
	ID3D11ShaderResourceView* GetNormalMap() {return this->normalMap->GetTexture();}

	std::vector<std::wstring>& GetTextureNameVector() { return this->textureNames; }
	Texture* GetTextureStruct() { return this->texture; }

	ID3D11Buffer& GetVertexBuffer() { return *this->vertexBuffer; }
	ID3D11Buffer& GetIndexBuffer() { return *this->indexBuffer; }

	DirectX::XMMATRIX GetWorldMatrix() { return this->world; }

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
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool CreateCube(ID3D11Device* device);
	bool CreateQuad(ID3D11Device* device);
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR filename);
	void ReleaseTexture();

	HRESULT hr;
	ID3D11Buffer *vertexBuffer, *indexBuffer;

	int vertexCount, indexCount;
	int subsetCount;

	std::string modelName;
	Texture* texture;
	Texture* normalMap;

	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	std::vector<XMFLOAT3> vertexPosArray;			// Used for CPU to do calculations on the Geometry
	std::vector<DWORD> vertexIndexArray;			// Also used for CPU calculations on geometry

	std::vector<Material> materials;
	std::vector<std::wstring> textureNames;
	std::vector<int> subsetIndexStart;
	std::vector<int> subsetMaterial;

	XMMATRIX world;
};