#include "EngineAndScene.h"

namespace dx = DirectX;

EngineAndScene::EngineAndScene()
{
	
	d3dHelperClass = nullptr;
	camera = nullptr;
	light = nullptr;
	model = nullptr;
	window = nullptr;
	deferredBuffer = nullptr;
	deferredShader = nullptr;
	lightShader = nullptr;
}

EngineAndScene::EngineAndScene(const EngineAndScene& other)
{

}

EngineAndScene::~EngineAndScene()
{

}

bool EngineAndScene::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool hr = false; // Default value

	
	
	d3dHelperClass = new D3DHelperClass;
	if(!d3dHelperClass)
	{
		return false;
	}

	hr = d3dHelperClass->Initialize(screenWidth, screenHeight, hwnd, fullScreen, screenDepth, screenNear);
	
	if(!hr)
	{
		std::cout << "Error creating d3dHelperClass object!" << std::endl;
		return false;
	}

	camera = new Camera;

	if(!camera)
	{
		std::cout << "Error creating camera object!" << std::endl;
		return false;
	}

	// Set camera positions and build matrices
	camera->SetPosition(0.0f, 0.0f, -5.0f);
	camera->Render();
	camera->RenderBaseViewMatrix();

	light = new LightClass;

	if(!light)
	{
		std::cout << "Error creating light object!" << std::endl;
		return false;
	}
	
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetDirection(0.0f, 0.0f, 1.0f);
	
	//create model object
	model = new ModelClass;
	if(!model)
	{
		std::cout << "Error creating model object!" << std::endl;
		return false;
	}

	// Initialize obj
	hr = model->Initialize(d3dHelperClass->GetDevice(), "Input link here", L"something");
	if(!hr)
	{
		std::cout << "Error! Could not initialize model!" << std::endl;
		return false;
	}

	// Create window class
	window = new OrthoWindowClass;
	if(!window)
	{
		std::cout << "Error! Could not create window object!" << std::endl;
		return false;
	}

	hr = window->Initialize(d3dHelperClass->GetDevice(), screenWidth, screenHeight);
	if(!hr)
	{
		std::cout << "Error! Could not initialize window" << std::endl;
		return false;
	}

	deferredBuffer = new DeferredRendering; 
	if(!deferredBuffer)
	{
		std::cout << "Error! Could not create deferred buffer object!" << std::endl;
		return false;
	}
	
	hr = deferredBuffer->Initialize(d3dHelperClass->GetDevice(), screenWidth, screenHeight, screenDepth, screenNear);
	if(!hr)
	{
		std::cout << "Error! Could not initialize deferred buffer" << std::endl;
		return false;
	}

	deferredShader = new DeferredShader;
	if(!deferredShader)
	{
		std::cout << "Error! Could not create deferred shader object!" << std::endl;
		return false;
	}

	hr = deferredShader->Initialize(d3dHelperClass->GetDevice(), hwnd);
	if(!hr)
	{
		std::cout << "Error! Could not initialize deferred shader" << std::endl;
		return false;
	}

	lightShader = new LightShader;
	if(!deferredShader)
	{
		std::cout << "Error! Could not create light shader object!" << std::endl;
		return false;
	}

	hr = lightShader->Initialize(d3dHelperClass->GetDevice(), hwnd);
		if(!hr)
	{
		std::cout << "Error! Could not initialize light shader" << std::endl;
		return false;
	}


	return true;
}
void EngineAndScene::Shutdown()
{
	// Release the light shader object.
	if(lightShader)
	{
		lightShader->ReleaseShader();
		delete lightShader;
		lightShader = 0;
	}

	// Release the deferred shader object.
	if(deferredShader)
	{
		deferredShader->ReleaseShader();
		delete deferredShader;
		deferredShader = 0;
	}

	// Release the deferred buffers object.
	if(deferredBuffer)
	{
		deferredBuffer->ReleaseStuff();
		delete deferredBuffer;
		deferredBuffer = 0;
	}

	// Release the full screen ortho window object.
	if(window)
	{
		window->ShutdownBuffers();
		delete window;
		window = 0;
	}

	// Release the model object.
	if(model)
	{
		model->Shutdown();
		delete model;
		model = 0;
	}

	// Release the light object.
	if(light)
	{
		delete light;
		light = 0;
	}

	// Release the camera object.
	if(camera)
	{
		delete camera;
		camera = 0;
	}
	
	// Release the D3D object.
	if(d3dHelperClass)
	{
		d3dHelperClass->Shutdown();
		delete d3dHelperClass;
		d3dHelperClass = 0;
	}

	return;
}

bool EngineAndScene::Frame()
{
	bool hr = false;

	
	// Read the user input.
	//hr = input->Frame();
	//if(!hr)
	//{
	//	return false;
	//}
	//
	//// Check if the user pressed escape and wants to exit the application.
	//if(input->IsEscapePressed() == true)
	//{
	//	return false;
	//}

	// Render the graphics.
	hr = Render();
	if(!hr)
	{
		return false;
	}
	
	return true;
}

bool EngineAndScene::Render()
{
	bool hr = false;
	//matrices
	dx::XMMATRIX world, view, ortho; 

	//render scene -> render buffers
	hr = RenderSceneToTexture();
	if(!hr)
	{
		std::cout << "Error! Could not render scene!";
		return false;
	}

	//clear scene
	d3dHelperClass->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	//setup for 2d-rendering
	d3dHelperClass->GetWorldMatrix(world);
	camera->GetViewMatrix(view);
	d3dHelperClass->GetOrthoMatrix(ortho);

	d3dHelperClass->TurnZBufferOff();

	window->Render(d3dHelperClass->GetDeviceContext());

	//render full screen ortho with deferred light shader and also render our buffers
	lightShader->Render(d3dHelperClass->GetDeviceContext(), window->GetIndexCount(), world, view, ortho, 
	deferredBuffer->GetShaderResourceView(0), deferredBuffer->GetShaderResourceView(1), light->GetDirection());

	d3dHelperClass->TurnZBufferOn();
	d3dHelperClass->EndScene();

	return true;
}


bool EngineAndScene::RenderSceneToTexture()
{
	//matrices
	dx::XMMATRIX world, view, projection;

	deferredBuffer->SetRenderTargets(d3dHelperClass->GetDeviceContext());

	deferredBuffer->ClearRenderTargets(d3dHelperClass->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	d3dHelperClass->GetWorldMatrix(world);
	camera->GetViewMatrix(view);
	d3dHelperClass->GetProjectionMatrix(projection);

	// Update rotation variable each frame
	static float rotation = 0.0f;
	rotation += (float)dx::XM_PI * 0.01f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	//rotate world matrix so object spins
	world = dx::XMMatrixRotationY(rotation);
	

	model->Render(d3dHelperClass->GetDeviceContext());

	deferredShader->Render(d3dHelperClass->GetDeviceContext(), model->GetIndexCount(), world, view, projection, model->GetTexture());

	d3dHelperClass->SetBackBufferRenderTarget();

	d3dHelperClass->ResetViewport();

	return true;
}

