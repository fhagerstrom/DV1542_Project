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

	// Set the initial position of the camera.
	camera->SetPosition(0.0f, 5.0f, -20.0f);
	camera->GetInputs(input);

	testCube = new Model;
	if (!testCube)
		return false;

	result = testCube->InitializeCube(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"assets/textures/iceGlass.jpg");
	if (!result)
	{
		MessageBox(NULL, L"Couldn't initialize cube. Check the file paths!", L"MODEL ERROR", MB_OK);
		return false;
	}

	plane = new Model;
	reader.LoadObj(dxHandler->GetDevice(), plane, L"assets/models/plane.obj", true, false);
	plane->SetWorldMatrix(XMMatrixTranslation(0.0f, -2.0f, 0.0f));
	plane->GetMaterials()[0].ambientMtl = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	// plane->GetTextureStruct()->SetName("ice");
	// plane->GetMaterials()[0].hasTexture = false;
	// plane->GetMaterials()[0].hasReflection = false;
	allModels.push_back(plane);

	torusRing = new Model;
	reader.LoadObj(dxHandler->GetDevice(), torusRing, L"assets/models/torusDonut.obj", true, false);
	torusRing->SetWorldMatrix(XMMatrixTranslation(-4.0f, 0.0f, 0.0f));
	// torusRing->GetTextureStruct()->SetName("ice");
	// torusRing->GetMaterials()[0].hasTexture = false;
	// torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(torusRing);

	suzanne = new Model;
	reader.LoadObj(dxHandler->GetDevice(), suzanne, L"assets/models/monkey_triangulated.obj", true, false);
	suzanne->SetWorldMatrix(XMMatrixTranslation(4.0f, 0.0f, 0.0f));
	// suzanne->GetTextureStruct()->SetName("ice");
	// suzanne->GetMaterials()[0].hasTexture = false;
	// suzanne->GetMaterials()[0].hasReflection = false;
	allModels.push_back(suzanne);

	sphere = new Model;
	reader.LoadObj(dxHandler->GetDevice(), sphere, L"assets/models/icoSphere.obj", true, false);
	sphere->SetWorldMatrix(XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	// sphere->GetTextureStruct()->SetName("ice");
	// sphere->GetMaterials()[0].hasTexture = false;
	// sphere->GetMaterials()[0].hasReflection = false;
	allModels.push_back(sphere);

	triforce = new Model;
	reader.LoadObj(dxHandler->GetDevice(), triforce, L"assets/models/triforce.obj", true, false);
	// WORLD MATRIX IS SET IN UPDATE DUE TO ROTATION

	triforce->GetMaterials()[0].ambientMtl = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
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

	// Initialize the color shader object.
	/*result = textureShader->Initialize(dxHandler->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader object.", L"SHADER ERROR", MB_OK);
		return false;
	}*/

	// REWRITTEN SHADER HANDLING TEST
	result = textureShader->InitializeShaders(dxHandler->GetDevice(), hwnd, L"DefaultVS.hlsl", L"DefaultPS.hlsl", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't initialize the shader object.", L"SHADER ERROR", MB_OK);
		return false;
	}

	result = textureShader->CreateDefaultInputLayout(dxHandler->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Couldn't create input layout!", L"INPUT LAYOUT ERROR", MB_OK);
		return false;
	}

	light = new Light;
	if (!light)
		return false;

	light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetSpecularColor(1.0f, 1.0f, 0.0f, 1.0f);
	light->SetDirection(5.0f, -10.0f, 20.0f);
	light->SetSpecularPower(8.0f);


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

	// Rotate model(s)
	rotTriforceAngle += 1.5f * dt;
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
	dxHandler->BeginScene(0.2f, 0.8f, 0.8f, 1.0f);

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
	// torusRing->Render(dxHandler->GetDeviceContext());
	// triforce->Render(dxHandler->GetDeviceContext());
	// suzanne->Render(dxHandler->GetDeviceContext());
	// sphere->Render(dxHandler->GetDeviceContext());

	// Render model using the texture shader
	/*result = textureShader->Render(dxHandler->GetDeviceContext(), triforce->GetIndexCount(), worldMatrix, view, projection, testCube->GetTexture(),
									light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), camera->GetPosition(), light->GetSpecularColor(), light->GetSpecularPower());*/


	// REWRITTEN SHADER RENDER FUNCTION TEST
	// result = textureShader->NewRender(dxHandler->GetDeviceContext(), triforce, view, projection, camera, light, dxHandler->GetMinMagMipSampler());

	for (int i = 0; i < allModels.size(); i++) 
	{
		allModels[i]->Render(dxHandler->GetDeviceContext());
		result = textureShader->NewRender(dxHandler->GetDeviceContext(), allModels[i], view, projection, camera, light, dxHandler->GetMinMagMipSampler());
		if (!result)
			return false;
	}

	//___________________________________________________________________________________________________

	// Present the rendered scene to the screen.
	dxHandler->EndScene();

	return true;
}
