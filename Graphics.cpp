#include "Graphics.h"


Graphics::Graphics(Input* input)
{
	this->input = input;
	this->directInput = input->GetDirectInput8();
	this->keyboard = input->GetDirectInput8Keyboard();
	this->mouse = input->GetDirectInput8Mouse();

	dxHandler = 0;
	camera = 0;
	testCube = 0;
	testQuad = 0;
	textureShader = 0;
	light = 0;
}

Graphics::~Graphics()
{
}


bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the DirectX object.
	dxHandler = new DxHandler;
	if(!dxHandler)
	{
		MessageBox(hwnd, L"Couldn't create DirectX handler object!", L"DX ERROR", MB_OK);
		return false;
	}

	// Initialize the DirectX object.
	result = dxHandler->Initialize(screenWidth, screenHeight, hwnd, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX!", L" DX ERROR", MB_OK);
		return false;
	}

	// Create the camera object.
	camera = new Camera;
	if (!camera)
	{
		return false;
	}

	// Set the initial position of the camera and call input function for movement.
	camera->SetPosition(0.0f, 10.0f, -50.0f);
	camera->GetInputs(input);

	/*testCube = new Model;
	if (!testCube)
		return false;

	result = testCube->InitializeCube(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"assets/textures/iceGlass.jpg");
	if (!result)
	{
		MessageBox(NULL, L"Couldn't initialize cube. Check the file paths!", L"MODEL ERROR", MB_OK);
		return false;
	}*/

	// Create light object.
	light = new Light;
	if (!light)
		return false;

	// Set lighting values.
	light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetLightPosition(0.0f, 10.0f, -5.0f);
	light->SetAttenuation(1.0f, 0.02f, 0.0f);
	light->SetLightRange(250.0f);
	lightVector.push_back(light);

	/*light1->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	light1->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	light1->SetLightPosition(0.0f, 10.0f, -5.0f);
	light1->SetAttenuation(1.0f, 0.02f, 0.0f);
	light1->SetLightRange(500.0f);*/

	cityTerrain = new Model;
	reader.LoadObj(dxHandler->GetDevice(), cityTerrain, L"assets/models/scifi_city.obj", true, false);
	cityTerrain->SetWorldMatrix(XMMatrixTranslation(0.0f, -2.0f, 0.0f));
	cityTerrain->GetMaterials()[0].ambientMtl = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
	cityTerrain->GetMaterials()[0].hasTexture = true;
	cityTerrain->GetMaterials()[0].hasReflection = false;
	allModels.push_back(cityTerrain);

	torusRing = new Model;
	reader.LoadObj(dxHandler->GetDevice(), torusRing, L"assets/models/torusDonut.obj", true, false);
	torusRing->SetWorldMatrix(XMMatrixTranslation(-4.0f, 0.0f, 0.0f));
	torusRing->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	torusRing->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	// torusRing->GetTextureStruct()->SetName("ice");
	// torusRing->GetMaterials()[0].hasTexture = false;
	// torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(torusRing);

	suzanne = new Model;
	reader.LoadObj(dxHandler->GetDevice(), suzanne, L"assets/models/monkey_triangulated.obj", true, false);
	suzanne->SetWorldMatrix(XMMatrixTranslation(4.0f, 0.0f, 0.0f));
	suzanne->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
	suzanne->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	// suzanne->GetTextureStruct()->SetName("ice");
	// suzanne->GetMaterials()[0].hasTexture = false;
	// suzanne->GetMaterials()[0].hasReflection = false;
	allModels.push_back(suzanne);

	sphere = new Model;
	reader.LoadObj(dxHandler->GetDevice(), sphere, L"assets/models/sphere.obj", true, false);
	sphere->SetWorldMatrix(XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	sphere->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
	sphere->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	// sphere->GetTextureStruct()->SetName("ice");
	// sphere->GetMaterials()[0].hasTexture = false;
	// sphere->GetMaterials()[0].hasReflection = false;
	allModels.push_back(sphere);

	triforce = new Model;
	reader.LoadObj(dxHandler->GetDevice(), triforce, L"assets/models/triforce.obj", true, false);
	// WORLD MATRIX IS SET IN UPDATE DUE TO ROTATION
	triforce->GetMaterials()[0].ambientMtl = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	triforce->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	// triforce->GetTextureStruct()->SetName("iceGlass");
	triforce->GetMaterials()[0].hasTexture = true;
	triforce->GetMaterials()[0].hasReflection = false;
	allModels.push_back(triforce);

	// Set model material
	// modelMtl.ambientMtl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Create the texture shader object.
	textureShader = new Shader;
	if (!textureShader)
	{
		return false;
	}

	deferredShader = new Shader;
	if (!deferredShader)
	{
		return false;
	}

	// Initialize texture shader
	result = textureShader->InitializeShaders(dxHandler->GetDevice(), hwnd, L"DefaultVS.hlsl", L"DefaultPS.hlsl", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize the shader object.", L"GRAPHICS ERROR", MB_OK);
		return false;
	}

	// Initialize deferred compute shader
	result = deferredShader->IntializeDeferredShaders(dxHandler->GetDevice(), hwnd, L"DefaultVS.hlsl", L"DeferredCS.hlsl", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize deferred shader object.", L"GRAPHICS ERROR", MB_OK);
		return false;
	}

	result = textureShader->CreateDefaultInputLayout(dxHandler->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't create input layout!", L"INPUT LAYOUT ERROR", MB_OK);
		return false;
	}

	// ImGui setup.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(dxHandler->GetDevice(), dxHandler->GetDeviceContext());
	ImGui::StyleColorsDark();

	return true;
}


void Graphics::Shutdown()
{
	// Release the texture shader object.
	if (textureShader)
	{
		textureShader->Shutdown();
		delete textureShader;
		textureShader = 0;
	}

	// Release the light object.
	if (light)
	{
		delete light;
		light = 0;
	}

	// Release the testCube object.
	if (testCube)
	{
		testCube->Shutdown();
		delete testCube;
		testCube = 0;
	}

	// Release the testCube object.
	if (testQuad)
	{
		testQuad->Shutdown();
		delete testQuad;
		testQuad = 0;
	}

	// Release the camera object.
	if (camera)
	{
		delete camera;
		camera = 0;
	}

	// Release the DX object.
	if(dxHandler)
	{
		dxHandler->Shutdown();
		delete dxHandler;
		dxHandler = 0;
	}

	return;
}

bool Graphics::Frame(float dt)
{
	bool result;

	// Update models and other moving objects
	Update(dt);

	// "Render" and update camera position
	camera->DetectInputs(dt);

	// Render the graphics scene.
	result = Render(dt);
	if (!result)
	{
		return false;
	}

	return true;
}

void Graphics::Update(float dt)
{
	// Acquire keyboard for inputs
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	/*if (keyboardState[DIK_I] & 0x80)
		DisableImgui();

	else
		EnableImgui();*/

	// Rotate model(s)
	rotTriforceAngle += 2.0f * dt;
	if (rotTriforceAngle > 2 * XM_PI)
		rotTriforceAngle = 0.0f;

	XMVECTOR rotTriforceAxis = XMVectorSet(0.0f, rotTriforceAngle, 0.0f, 0.0f);
	XMMATRIX rotTriforceRotation = XMMatrixRotationRollPitchYawFromVector(rotTriforceAxis);
	XMMATRIX triforceTranslation = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	triforce->SetWorldMatrix(rotTriforceRotation * triforceTranslation);
}

bool Graphics::Render(float dt)
{
	bool result;
	XMMATRIX view, projection;

	// Clear the buffers to begin the scene.
	dxHandler->BeginScene(0.3f, 0.3f, 0.3f, 1.0f);

	// PUT RENDERS BELOW HERE
	//___________________________________________________________________________________________________

	// Get the world, view, and projection matrices from the camera and DX objects.
	camera->GetViewMatrix(view);
	dxHandler->GetProjectionMatrix(projection);

	// CALL RENDER FUNCTION FOR MODELS
	//___________________________________________________________________________________________________

	// Put the testCube vertex and index buffers on the graphics pipeline to prepare them for drawing
	// testCube->Render(dxHandler->GetDeviceContext());
	// testQuad->Render(dxHandler->GetDeviceContext());

	// INDIVIDUAL MODEL RENDERS
	// cityTerrain->Render(dxHandler->GetDeviceContext());
	// torusRing->Render(dxHandler->GetDeviceContext());
	// triforce->Render(dxHandler->GetDeviceContext());
	// suzanne->Render(dxHandler->GetDeviceContext());
	// sphere->Render(dxHandler->GetDeviceContext());

	// REWRITTEN SHADER RENDER FUNCTION
	//result = textureShader->NewRender(dxHandler->GetDeviceContext(), cityTerrain, view, projection, camera, light, dxHandler->GetMinMagMipSampler());

	// COMPUTE SHADER RENDER
	/*result = deferredShader->NewRender(dxHandler->GetDeviceContext(), torusRing, view, projection, 
		camera, light, dxHandler->GetMinMagMipSampler());*/

	for (int i = 0; i < allModels.size(); i++) 
	{
		allModels[i]->Render(dxHandler->GetDeviceContext());
		result = textureShader->NewRender(dxHandler->GetDeviceContext(), allModels[i], view, projection, camera, light, dxHandler->GetMinMagMipSampler());
		if (!result)
			return false;
	}

	//___________________________________________________________________________________________________

	// CALL IMGUI FRAME PROPERTIES
	//___________________________________________________________________________________________________

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Test window.
	ImGui::Begin("CHANGE PROPERTIES");
	ImGui::DragFloat3("Light pos (x, y, z): ", movableLightPos, 0.1f, -50.0f, 50.0f);
	light->SetLightPosition(movableLightPos[0], movableLightPos[1], movableLightPos[2]);
	// light1->SetLightPosition(movableLightPos[0], movableLightPos[1], movableLightPos[2]);

	ImGui::End();

	// Assemble draw data from Imgui and draw window(s).
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// TODO: Fix toggling on and off.
	/*if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}*/

	//___________________________________________________________________________________________________

	// Present the rendered scene to the screen.
	dxHandler->EndScene();

	return true;
}
