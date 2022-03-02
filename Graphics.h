#pragma once

#include "DxHandler.h"
#include "cameraclass.h"
#include "Model.h"
#include "Shaders.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	DxHandler* dxHandler;
	Camera* camera;
	Model* testCube;
	Shader* textureShader;

	// Rotation related variables for models that should.
	float rotCubeAngle = 0.0f;
	float rotSpeed = 0.0001f;
};