#include "Graphics.h"


Graphics::Graphics()
{
	dxHandler = 0;
	camera = 0;
	testCube = 0;
	textureShader = 0;
}


Graphics::Graphics(const Graphics& other)
{
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
		return false;
	}

	// Initialize the DirectX object.
	result = dxHandler->Initialize(screenWidth, screenHeight, hwnd, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	camera = new Camera;
	if (!camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera->SetPosition(0.0f, 0.0f, -5.0f);

	testCube = new Model;
	result = testCube->InitializeCube(dxHandler->GetDevice(), dxHandler->GetDeviceContext(), L"Textures/coollink.png");
	if (!result)
		return false;

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

	// Release the testCube object.
	if (testCube)
	{
		testCube->Shutdown();
		delete testCube;
		testCube = 0;
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

bool Graphics::Frame()
{
	bool result;

	// static float rotationValue = 0.0001f;

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool Graphics::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	dxHandler->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	// PUT RENDERS BELOW HERE
	//___________________________________________________________________________________________________

	// Generate the view matrix based on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera and DX objects.
	dxHandler->GetWorldMatrix(worldMatrix);
	camera->GetViewMatrix(viewMatrix);
	dxHandler->GetProjectionMatrix(projectionMatrix);

	// Rotation n stuff
	rotCubeAngle += XM_PI * rotSpeed;
	if (rotCubeAngle > 2 * XM_PI)
		rotCubeAngle = 0.0f;

	XMVECTOR rotCubeAxis = DirectX::XMVectorSet(rotCubeAngle, rotCubeAngle, 0.0f, 0.0f);
	XMMATRIX rotCubeRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(rotCubeAxis);

	// TODO: SEPARATE WORLD MATRIX BETWEEN MODELS WHEN RENDERING MORE OF THEM.
	worldMatrix = rotCubeRotation * XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	// Put the testCube vertex and index buffers on the graphics pipeline to prepare them for drawing.
	testCube->Render(dxHandler->GetDeviceContext());

	// Render the testCube using the texture shader.
	result = textureShader->Render(dxHandler->GetDeviceContext(), testCube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, testCube->GetTexture());
	if (!result)
	{
		return false;
	}

	//___________________________________________________________________________________________________

	// Present the rendered scene to the screen.
	dxHandler->EndScene();

	return true;
}
