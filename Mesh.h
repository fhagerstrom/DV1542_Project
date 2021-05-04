#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "Vertex.h"


class Mesh
{
private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	int numIndices;

	void CalculateTangents(Vertex* vertices, int numVertices, unsigned int* indices, int numIndices);
	void CreateBuffers(ID3D11Device* device, Vertex* verticeArray, int numVerts, unsigned int* indexArray, int numIndices);

public:
	Mesh(ID3D11Device* device, Vertex* verticeArray, int numVertices, unsigned int* indexArray, int numIndices);
	Mesh(ID3D11Device* device, const char* objFile);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer() { return vBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return iBuffer; }
	int GetIndexCount() { return numIndices; }
};

