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
	camera->SetPosition(0.0f, 0.0f, -6.0f);
	// camera->SetRotation(0.0f, 2.0f, 0.0f);

	testCube = new Model;
	if (!testCube)
		return false;

	result = testCube->InitializeCube(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"assets/textures/iceWall.png");
	if (!result)
	{
		MessageBox(NULL, L"Couldn't initialize cube. Check the file paths!", L"MODEL ERROR", MB_OK);
		return false;
	}

	objTest = new Model;
	reader.LoadObj(dxHandler->GetDevice(), objTest, L"assets/models/monkey.obj", true, false);
	//objTest->GetMaterials()[0].hasTexture = false;
	//objTest->GetMaterials()[0].hasReflection = false;
	allModels.push_back(objTest);

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

	// Create the texture shader object.
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

	light = new Light;
	if (!light)
		return false;

	light->SetAmbientColor(0.25f, 0.25f, 0.25f, 1.0f);
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetDirection(0.0f, 10.0f, 20.0f);
	light->SetSpecularPower(32.0f);
	light->SetSpecularColor(0.0f, 0.0f, 0.0f, 0.0f); // TODO: FIX BUG WITH SPECULAR COLOR, DOESNT CHANGE OR DISAPPPEAR WHEN COMMENTED


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

	// Clear the buffers to begin the scene.
	dxHandler->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	// PUT RENDERS BELOW HERE
	//___________________________________________________________________________________________________

	// Generate the view matrix based on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera and DX objects.
	camera->GetViewMatrix(viewMatrix);
	dxHandler->GetWorldMatrix(worldMatrix);
	dxHandler->GetProjectionMatrix(projectionMatrix);

	// Rotation n stuff
	rotCubeAngle += 1.0f * dt;
	if (rotCubeAngle > 2 * XM_PI)
		rotCubeAngle = 0.0f;

	XMVECTOR rotCubeAxis = DirectX::XMVectorSet(0.0f, rotCubeAngle, rotCubeAngle, 0.0f);
	XMMATRIX rotCubeRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(rotCubeAxis);

	rotQuadAngle += 0.4f * dt;
	if (rotQuadAngle > 2 * XM_PI)
		rotQuadAngle = 0.0f;

	XMVECTOR rotQuadAxis = DirectX::XMVectorSet(0.0f, rotQuadAngle, 0.0f, 0.0f);
	XMMATRIX rotQuadRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(rotQuadAxis);

	// TODO: Make a SetWorldMatrix function for the Model class.
	worldMatrix = rotCubeRotation * XMMatrixTranslation(0.0f, 0.0f, 0.0f);


	// CALL RENDER FUNCTION FOR MODELS
	//___________________________________________________________________________________________________

	// Put the testCube vertex and index buffers on the graphics pipeline to prepare them for drawing.
	testCube->Render(dxHandler->GetDeviceContext());
	// testQuad->Render(dxHandler->GetDeviceContext());

	// Render the testCube using the texture shader.
	result = textureShader->Render(dxHandler->GetDeviceContext(), objTest->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, testCube->GetTexture(), 
									light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), camera->GetPosition(), light->GetSpecularColor(), light->GetSpecularPower());

	/*for (int i = 0; i < allModels.size(); i++) 
	{
		allModels[i]->Render(dxHandler->GetDeviceContext());
	}*/

	if (!result)
	{
		return false;
	}

	//___________________________________________________________________________________________________

	// Present the rendered scene to the screen.
	dxHandler->EndScene();

	return true;
}
