#pragma once

#include "Camera.h"
#include "ObjReader.h"
#include "Model.h"
#include "Shaders.h"
#include "Light.h"
#include "Timer.h"
#include "ParticleSystem.h"

// IMGUI INCLUDES
#include "assets/imgui/imgui.h"
#include "assets/imgui/imgui_impl_dx11.h"
#include "assets/imgui/imgui_impl_win32.h"

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

	// Functions to control ImGui
	void EnableImgui() { imguiEnabled = true; };
	void DisableImgui() { imguiEnabled = false; };
	bool IsImguiEnabled() { return imguiEnabled; };

private:
	bool Render(float dt);

	DxHandler* dxHandler;
	
	Camera* camera;
	Light* light;
	Light* light1;
	int nrOfLights;

	Model* cityTerrain;
	Model* cornellBox;
	Model* torusRing;
	Model* suzanne;
	Model* sphere;
	Model* triforce;
	Model* n64Logo;

	ParticleSystem* particleSystem;
	Particle* particles;

	Shader* textureShader;
	Shader* deferredShader;
	Shader* particleShader;

	ObjReader reader;

	std::vector<Model*> allModels;
	std::vector<Light*> lightVector;

	// Input devices
	Input* input;
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;
	BYTE keyboardState[256];

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// ImGui related variables.
	// Bool to check enable/disable status
	bool imguiEnabled = true;

	// To be able to move light.
	float movableLightPos[3];

	// Rotation related variables for models that will rotate.
	float rotTriforceAngle = 0.0f;
	float rotN64Angle = 0.0f;
};