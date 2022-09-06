#include "Graphics.h"


Graphics::Graphics()
{
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
	camera->SetPosition(0.0f, 0.0f, -10.0f);
	// camera->SetRotation(0.0f, 2.0f, 0.0f);

	testCube = new Model;
	if (!testCube)
		return false;

	result = testCube->InitializeCube(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"assets/textures/spaceWallpaper.jpg");
	if (!result)
	{
		MessageBox(NULL, L"Couldn't initialize cube. Check the file paths!", L"MODEL ERROR", MB_OK);
		return false;
	}

	torusRing = new Model;
	reader.LoadObj(dxHandler->GetDevice(), torusRing, L"assets/models/torusDonut.obj", true, false);
	torusRing->SetWorldMatrix(XMMatrixTranslation(-2.0f, 0.0f, 0.0f));
	// torusRing->GetTextureStruct()->SetName("ice");
	// torusRing->GetMaterials()[0].hasTexture = false;
	//torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(torusRing);

	suzanne = new Model;
	reader.LoadObj(dxHandler->GetDevice(), suzanne, L"assets/models/monkey_triangulated.obj", true, false);
	torusRing->SetWorldMatrix(XMMatrixTranslation(2.0f, 0.0f, 0.0f));
	// torusRing->GetTextureStruct()->SetName("ice");
	// torusRing->GetMaterials()[0].hasTexture = false;
	//torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(suzanne);

	sphere = new Model;
	reader.LoadObj(dxHandler->GetDevice(), sphere, L"assets/models/icoSphere.obj", true, false);
	// torusRing->GetTextureStruct()->SetName("ice");
	// torusRing->GetMaterials()[0].hasTexture = false;
	//torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(sphere);

	triforce = new Model;
	reader.LoadObj(dxHandler->GetDevice(), triforce, L"assets/models/triforce.obj", true, false);
	torusRing->SetWorldMatrix(XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	// torusRing->GetTextureStruct()->SetName("ice");
	// torusRing->GetMaterials()[0].hasTexture = false;
	//torusRing->GetMaterials()[0].hasReflection = false;
	allModels.push_back(triforce);

	// Set model material
	//modelMtl.ambientMtl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	/*testQuad = new Model;
	if (!testQuad)
		return false;

	result = testQuad->InitializeQuad(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"assets/textures/spaceWallpaper.jpg");
	if (!result)
	{
		MessageBox(NULL, L"Couldn't initialize quad. Check the file paths!", L"MODEL ERROR", MB_OK);
		return false;
	}*/

	// Create the textureSRV shader object.
	textureShader = new Shader;
	if (!textureShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = textureShader->Initialize(dxHandler->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader object.", L"SHADER ERROR", MB_OK);
		return false;
	}

	// REWRITTEN SHADER HANDLING TEST
	/*result = textureShader->InitializeShaders(dxHandler->GetDevice(), hwnd, L"DefaultVS.hlsl", L"DefaultPS.hlsl", "main", "main");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader object.", L"SHADER ERROR", MB_OK);
		return false;
	}*/

	/*result = textureShader->CreateDefaultInputLayout(dxHandler->GetDevice());
	if (!result)
		return false;*/




	light = new Light;
	if (!light)
		return false;

	light->SetAmbientColor(0.25f, 0.25f, 0.25f, 1.0f);
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f); // TODO: FIX BUG WITH SPECULAR COLOR, DOESNT CHANGE OR DISAPPPEAR WHEN COMMENTED
	light->SetDirection(-5.0f, 1.0f, 1.0f);
	light->SetSpecularPower(16.0f);


	return true;
}


void Graphics::Shutdown()
{
	// Release the textureSRV shader object.
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
	// Update(dt);

	// Render the graphics scene.
	result = Render(dt);
	if (!result)
	{
		return false;
	}

	return true;
}

//void Graphics::Update(float dt)
//{
//
//
//}

bool Graphics::Render(float dt)
{
	bool result;
	XMMATRIX view, projection;

	// Clear the buffers to begin the scene.
	dxHandler->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	// PUT RENDERS BELOW HERE
	//___________________________________________________________________________________________________

	// Generate the view matrix based on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera and DX objects.
	camera->GetViewMatrix(viewMatrix);
	dxHandler->GetProjectionMatrix(projectionMatrix);

	// Rotation n stuff
	rotModelAngle += 1.0f * dt;
	if (rotModelAngle > 2 * XM_PI)
		rotModelAngle = 0.0f;

	XMVECTOR rotModelAxis = DirectX::XMVectorSet(0.0f, rotModelAngle, 0.0f, 0.0f);
	XMMATRIX rotModelRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(rotModelAxis);

	/*rotQuadAngle += 0.4f * dt;
	if (rotQuadAngle > 2 * XM_PI)
		rotQuadAngle = 0.0f;

	XMVECTOR rotQuadAxis = DirectX::XMVectorSet(0.0f, rotQuadAngle, 0.0f, 0.0f);
	XMMATRIX rotQuadRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(rotQuadAxis);*/

	// TODO: Make a SetWorldMatrix function for the Model class.
	worldMatrix = rotModelRotation * XMMatrixTranslation(0.0f, 0.0f, 0.0f);


	// CALL RENDER FUNCTION FOR MODELS
	//___________________________________________________________________________________________________

	// Put the testCube vertex and index buffers on the graphics pipeline to prepare them for drawing
	// testCube->Render(dxHandler->GetDeviceContext());
	// testQuad->Render(dxHandler->GetDeviceContext());

	// torusRing->Render(dxHandler->GetDeviceContext());
	triforce->Render(dxHandler->GetDeviceContext());
	// suzanne->Render(dxHandler->GetDeviceContext());
	// sphere->Render(dxHandler->GetDeviceContext());

	// Render model using the textureSRV shader
	result = textureShader->Render(dxHandler->GetDeviceContext(), triforce->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, testCube->GetTexture(), 
									light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), camera->GetPosition(), light->GetSpecularColor(), light->GetSpecularPower());

	/*result = textureShader->Render(dxHandler->GetDeviceContext(), suzanne->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, testCube->GetTexture(),
		light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), camera->GetPosition(), light->GetSpecularColor(), light->GetSpecularPower());*/



	//for (int i = 0; i < allModels.size(); i++) 
	//{
	//	allModels[i]->Render(dxHandler->GetDeviceContext());
	//	//result = textureShader->NewRender(dxHandler->GetDeviceContext(), allModels[i], view, projection, camera, light, dxHandler->GetMinMagMipSampler());
	//	if (!result)
	//		return false;
	//}

	if (!result)
	{
		return false;
	}

	//___________________________________________________________________________________________________

	// Present the rendered scene to the screen.
	dxHandler->EndScene();

	return true;
}
