#pragma once
//const int BUFFER_COUNT = 3;

#include "DXInit.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "EngineEntity.h"
#include "Camera.h"
#include "Vertex.h"
#include "Lights.h"


class Renderer
{
private:
	ID3D11Device* device;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	PointLight pointLight1;
	PointLight pointLight2;
	PointLight pointLight3;
	PointLight pointLight4;

	SpotLight spotLight1;
	SpotLight spotLight2;

public:
	//Renderer();
	~Renderer();

	void SetLights();
};