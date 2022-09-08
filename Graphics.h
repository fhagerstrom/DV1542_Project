#pragma once

#include "DxHandler.h"
#include "Camera.h"
#include "ObjReader.h"
#include "Model.h"
#include "Shaders.h"
#include "Light.h"
#include "Timer.h"

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics(Input* input);
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float dt);
	void Update(float dt);

private:
	bool Render(float dt);

	DxHandler* dxHandler;
	Camera* camera;

	Model* testCube;
	Model* testQuad;
	Model* plane;
	Model* torusRing;
	Model* suzanne;
	Model* sphere;
	Model* triforce;

	Material modelMtl;

	Shader* textureShader;
	Light* light;
	ObjReader reader;

	std::vector<Model*> allModels;

	/* Input devices */
	Input* input;
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Rotation related variables for models that will rotate.
	float rotTriforceAngle = 0.0f;
};