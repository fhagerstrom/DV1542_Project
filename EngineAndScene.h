#pragma once

#include <iostream>
#include <directxmath.h>

#include "DeferredBuffer.h"
#include "LightShaderClass.h"
#include "D3DHelperClass.h"
#include "Camera.h"
#include "D3D11Setup.h"
#include "DeferredShader.h"
#include "Pipeline.h"
#include "LightClass.h"
#include "ModelClass.h"
#include "OrthoWindowClass.h"

class EngineAndScene
{
private:
	bool Render();
	bool RenderSceneToTexture();


	D3DHelperClass* d3dHelperClass;
	Camera* camera;

	//light class (regular directional light object)
	LightClass* light;

	//model that will be rendering
	ModelClass* model;

	//2d window that will be used with deferred rendering
	OrthoWindowClass* window;

	//deferred buffer
	DeferredRendering* deferredBuffer;

	//deferred shader and light shader
	DeferredShader* deferredShader;
	LightShader* lightShader;

public:
	EngineAndScene();
	EngineAndScene(const EngineAndScene&);
	~EngineAndScene();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	bool Frame();
	void Shutdown();
	const bool fullScreen = true;
	const float screenDepth = 1000.0f;
	const float screenNear = 0.1f;
};

