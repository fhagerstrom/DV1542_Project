#pragma once
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <assert.h>
#include <string>

#include "Model.h"

using namespace DirectX;

class ObjReader
{
private:
	HRESULT hr;

public:
	ObjReader();
	~ObjReader();

	bool LoadObj(ID3D11Device* device, Model* model, std::wstring modelFile, bool rightHandCoord, bool calcNormals);
};