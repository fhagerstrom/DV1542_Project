#pragma once
#include <d3d11.h>
#include <fstream>
#include <stdio.h>
#include <DirectXMath.h>

#include "Vertex.h"

namespace dx = DirectX;

// @TODO: 
// - Rework code to use vectors instead of arrays.
// - Use Vertex Struct instead of HeightmapType and ModelType.
// - Fix mesh for terrain so it can be drawn.

struct HeightMapType
{
	float x, y, z;
};

struct ModelType
{
	float x, y, z;
};

class Terrain
{
private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

	int vCount, iCount;
	int terrainHeight, terrainWidth;

	float heightScale;
	char* terrainFile;

	HeightMapType* heightMap;
	ModelType* terrainModel;

public:
	int getIndexCount;

	Terrain();
	Terrain(const Terrain& other);
	~Terrain();

	bool LoadSetup(const char* filename);
	bool LoadHeightMap();
	bool BuildTerrainModel();
	bool InitBuffers(ID3D11Device* device);

	void RenderTerrain(ID3D11DeviceContext* context);
	void SetTerrainCoordinates();


};
