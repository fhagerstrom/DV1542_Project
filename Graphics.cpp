#include "Graphics.h"


Graphics::Graphics(Input* input)
{
	this->input = input;
	this->directInput = input->GetDirectInput8();
	this->keyboard = input->GetDirectInput8Keyboard();
	this->mouse = input->GetDirectInput8Mouse();

	dxHandler = 0;
	camera = 0;
	textureShader = 0;
	deferredShader = 0;
	light = 0;

	maxParticles = 500;
	quadLength = 50;
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

	// Create light object.
	light = new Light;
	if (!light)
		return false;

	light1 = new Light;
	if (!light)
		return false;

	// Set lighting values.
	light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetLightPosition(0.0f, 0.0f, 0.0f);
	light->SetAttenuation(1.0f, 0.02f, 0.0f);
	light->SetLightRange(500.0f);
	lightVector.push_back(light);

	light1->SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	light1->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	light1->SetLightPosition(0.0f, 10.0f, 15.0f);
	light1->SetAttenuation(1.0f, 0.02f, 0.0f);
	light1->SetLightRange(500.0f);
	// lightVector.push_back(light1);

	cityTerrain = new Model;
	reader.LoadObj(dxHandler->GetDevice(), cityTerrain, L"assets/models/scifi_city.obj", true, false);
	cityTerrain->SetWorldMatrix(XMMatrixTranslation(0.0f, -2.0f, 0.0f));
	cityTerrain->GetMaterials()[0].ambientMtl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cityTerrain->GetMaterials()[0].hasTexture = true;
	cityTerrain->GetMaterials()[0].hasReflection = false;
	allModels.push_back(cityTerrain);

	cornellBox = new Model;
	reader.LoadObj(dxHandler->GetDevice(), cornellBox, L"assets/models/cornellBox.obj", true, false);
	cornellBox->SetWorldMatrix(XMMatrixTranslation(0.0f, 30.0f, 0.0f));
	cornellBox->GetMaterials()[0].ambientMtl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cornellBox->GetMaterials()[0].hasTexture = false;
	cornellBox->GetMaterials()[0].hasReflection = false;
	allModels.push_back(cornellBox);

	torusRing = new Model;
	reader.LoadObj(dxHandler->GetDevice(), torusRing, L"assets/models/torusDonut.obj", true, false);
	torusRing->SetWorldMatrix(XMMatrixTranslation(-4.0f, 0.0f, 0.0f));
	torusRing->GetMaterials()[0].ambientMtl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	torusRing->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	torusRing->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	torusRing->GetMaterials()[0].hasTexture = true;
	torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(torusRing);

	suzanne = new Model;
	reader.LoadObj(dxHandler->GetDevice(), suzanne, L"assets/models/monkey_triangulated.obj", true, false);
	suzanne->SetWorldMatrix(XMMatrixTranslation(4.0f, 0.0f, 0.0f));
	suzanne->GetMaterials()[0].ambientMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	suzanne->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	suzanne->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	suzanne->GetMaterials()[0].hasTexture = false;
	suzanne->GetMaterials()[0].hasReflection = false;
	allModels.push_back(suzanne);

	sphere = new Model;
	reader.LoadObj(dxHandler->GetDevice(), sphere, L"assets/models/sphere.obj", true, false);
	sphere->SetWorldMatrix(XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(-20.0f, 40.0f, 20.0f));
	sphere->GetMaterials()[0].ambientMtl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sphere->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
	sphere->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	sphere->GetMaterials()[0].hasTexture = true;
	sphere->GetMaterials()[0].hasReflection = false;
	allModels.push_back(sphere);

	triforce = new Model;
	reader.LoadObj(dxHandler->GetDevice(), triforce, L"assets/models/triforce.obj", true, false);
	// WORLD MATRIX IS SET IN UPDATE DUE TO ROTATION
	triforce->GetMaterials()[0].ambientMtl = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	triforce->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f);
	triforce->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	triforce->GetMaterials()[0].hasTexture = true;
	triforce->GetMaterials()[0].hasReflection = false;
	allModels.push_back(triforce);

	n64Logo = new Model;
	reader.LoadObj(dxHandler->GetDevice(), n64Logo, L"assets/models/n64Logo.obj", true, false);
	// WORLD MATRIX IS SET IN UPDATE DUE TO ROTATION
	n64Logo->GetMaterials()[0].ambientMtl = XMFLOAT4(0.5f, 0.75f, 0.5f, 1.0f);
	n64Logo->GetMaterials()[0].diffuseMtl = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	n64Logo->GetMaterials()[0].specularMtl = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	n64Logo->GetMaterials()[0].hasTexture = false;
	n64Logo->GetMaterials()[0].hasReflection = false;
	allModels.push_back(n64Logo);

	// Create the texture shader object.
	textureShader = new Shader;
	if (!textureShader)
	{
		return false;
	}

	// Create deferred shader object.
	deferredShader = new Shader;
	if (!deferredShader)
	{
		return false;
	}

	// Create particle shader object.
	particleShader = new Shader;
	if (!particleShader)
	{
		return false;
	}

	// Initialize texture shader
	/*result = textureShader->InitializeShaders(dxHandler->GetDevice(), hwnd, L"DefaultVS.hlsl", L"DefaultPS.hlsl", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize the shader object.", L"GRAPHICS ERROR", MB_OK);
		return false;
	}*/

	// Initialize deferred shader
	result = deferredShader->IntializeDeferredShaders(dxHandler->GetDevice(), dxHandler, hwnd, L"DeferredVS.hlsl", L"DeferredPS.hlsl", L"DeferredCS.hlsl", "main", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize deferred shader object.", L"GRAPHICS ERROR", MB_OK);
		return false;
	}

	// Create the input layout for all models
	result = deferredShader->CreateDefaultInputLayout(dxHandler->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't create default input layout!", L"INPUT LAYOUT ERROR", MB_OK);
		return false;
	}

	// Initialize particle shader
	result = particleShader->InitializeParticleShaders(dxHandler->GetDevice(), hwnd, L"ParticleVS.hlsl", L"ParticlePS.hlsl", L"ParticleCS.hlsl", L"ParticleGS.hlsl", "main", "main", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize particle shader object.", L"GRAPHICS ERROR", MB_OK);
		return false;
	}

	// Create particle system object.
	particleSystem = new ParticleSystem;
	if (!particleSystem)
	{
		return false;
	}

	// Create particle object
	particles = new Particle;
	if (!particles)
	{
		return false;
	}

	// Initialize particle system.
	result = particleSystem->Initialize(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"assets/textures/rainDrop.png", particles, maxParticles);
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize particle system!", L"PARTICLE ERROR", MB_OK);
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

	if (deferredShader)
	{
		deferredShader->Shutdown();
		delete deferredShader;
		deferredShader = 0;
	}

	// Release the light object.
	if (light)
	{
		delete light;
		light = 0;
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
	XMMATRIX view;
	camera->GetViewMatrix(view);

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

	XMVECTOR rotTriforceAxis = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX rotTriforceRotation = XMMatrixRotationRollPitchYawFromVector(rotTriforceAxis);
	XMMATRIX triforceTranslation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	triforce->SetWorldMatrix(rotTriforceRotation * triforceTranslation);

	rotN64Angle += 1.5f * dt;
	if (rotN64Angle > 2 * XM_PI)
		rotN64Angle = 0.0f;

	XMVECTOR rotN64Axis = XMVectorSet(0.0f, rotN64Angle, 0.0f, 0.0f);
	XMMATRIX rotN64Rotation = XMMatrixRotationRollPitchYawFromVector(rotN64Axis);
	XMMATRIX n64Translation = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	n64Logo->SetWorldMatrix(rotN64Rotation * n64Translation);

	// PARTICLES
	particleShader->UpdateParticleCBuffers(dxHandler->GetDeviceContext(), particleSystem, view, camera->GetPosition(), dt);
}

bool Graphics::Render(float dt)
{
	bool result;
	XMMATRIX view, projection;

	// Clear the buffers to begin the scene.
	dxHandler->BeginScene(0.1f, 0.0f, 0.3f, 1.0f);


	// PUT RENDERS BELOW HERE
	//___________________________________________________________________________________________________

	// Get the view and projection matrices from the camera and DX object.
	camera->GetViewMatrix(view);
	dxHandler->GetProjectionMatrix(projection);

	// CALL INDIVIDUAL RENDER FUNCTION FOR MODELS
	//___________________________________________________________________________________________________

	// cityTerrain->Render(dxHandler->GetDeviceContext());
	// torusRing->Render(dxHandler->GetDeviceContext());
	// triforce->Render(dxHandler->GetDeviceContext());
	// suzanne->Render(dxHandler->GetDeviceContext());
	// sphere->Render(dxHandler->GetDeviceContext());

	// REGULAR "TEXTURE" SHADER / FORWARD RENDERING
 	// result = textureShader->Render(dxHandler->GetDeviceContext(), cityTerrain, view, projection, camera, light, dxHandler->GetSampler());

	// DEFERRED RENDERING
	// Bind first (geometry) pass for the deferred shading
	deferredShader->BindGeometryPass(dxHandler->GetDeviceContext());

	// Loop through all models and render them.
	for (int i = 0; i < allModels.size(); i++) 
	{
		allModels[i]->Render(dxHandler->GetDeviceContext());
		result = deferredShader->Render(dxHandler->GetDeviceContext(), allModels[i], view, projection, camera, light, dxHandler->GetSampler());
		if (!result)
			return false;
	}

	// PARTICLES
	result = particleShader->RenderParticles(dxHandler->GetDeviceContext(), particleSystem, maxParticles, projection, quadLength);
	if (!result)
		return false;

	particleShader->UpdateParticleCompute(dxHandler->GetDeviceContext(), particleSystem);

	particleSystem->Render(dxHandler->GetDeviceContext(), maxParticles);


	// Bind the second (light) pass for the deferred shading
	deferredShader->BindLightPass(dxHandler->GetDeviceContext());

	//___________________________________________________________________________________________________

	// CALL IMGUI FRAME PROPERTIES
	//___________________________________________________________________________________________________

	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//// Test window.
	//ImGui::Begin("CHANGE PROPERTIES");
	//ImGui::DragFloat3("Light pos (x, y, z): ", movableLightPos, 0.1f, -50.0f, 50.0f);
	//light->SetLightPosition(movableLightPos[0], movableLightPos[1], movableLightPos[2]);
	//light1->SetLightPosition(movableLightPos[0], movableLightPos[1], movableLightPos[2]);

	// ImGui::End();

	// Assemble draw data from Imgui and draw window(s).
	/*ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());*/

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
