#pragma once
#include "Vertex.h"
#include "Texture.h"
#include "DirectXHelper.h"


struct Mesh
{
private:
	bool LoadModel(std::string name);
	bool LoadMaterial(ID3D11Device* device, std::string name);
	std::string name;
public:

	Material material;
	ID3D11Buffer* mtlBuffer;
	Mesh() = default;
	Mesh(ID3D11Device* device, std::string name);
	~Mesh();
	ID3D11Buffer* GetMTLBuffer();
	int vertexCount = 0;
	// Struct from geometry
	std::vector<Face> faces;
	std::vector<XMFLOAT3> positions; //stores positions of every vertex, this is used in the shadow vertex shader
	void Shutdown();
};