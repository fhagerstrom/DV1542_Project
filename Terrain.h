#pragma once
#include <Windows.h>
#include <vector>
#include <DirectXMath.h>

namespace dx = DirectX;

struct HeightMapInfo
{
	int terrainWidth;
	int terrainHeight;
	dx::XMFLOAT3* heightMap;
};

class Terrain
{
private:
	int numFaces;
	int numVertices;

public:
	Terrain();
	~Terrain();

	bool LoadHeightMap(char* filename, HeightMapInfo& hmInfo);
};
