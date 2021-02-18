#include "EngineAndScene.h"

EngineAndScene::EngineAndScene()
{
	
	d3dHelperClass = 0;
	camera = 0;
	light = 0;
	model = 0;
	FullScreenWindow = 0;
	deferredBuffer = 0;
	deferredShader = 0;
	lightShader = 0;
}

EngineAndScene::EngineAndScene(const EngineAndScene& other)
{

}

EngineAndScene::~EngineAndScene()
{

}

bool EngineAndScene::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool hr = false; //default value

	
	
	d3dHelperClass = new D3DHelperClass;
	if(!d3dHelperClass)
	{
		return false;
	}

	hr = d3dHelperClass->Initialize(screenWidth, screenHeight, hwnd, fullScreen, screenDepth, screenNear);
	
	if(!hr)
	{
		std::cerr << "Error creating d3dHelperClass object!" << std::endl;
		return false;
	}

	camera = new Camera(0.0f, 0.0f, -5.0f);
	camera->UpdateProjMatrix((float)screenWidth / screenHeight);

	if(!camera)
	{
		std::cerr << "Error creating camera object!" << std::endl;
		return false;
	}

	camera->Update(1.0f);
	camera->UpdateViewMatrix();

	light = new LightClass;

	if(!light)
	{
		std::cerr << "Error creating light object!" << std::endl;
		return false;
	}
	
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetDirection(0.0f, 0.0f, 1.0f);
	
	//create model object
	model = new ModelClass;
	if(!model)
	{
		std::cerr << "Error creating model object!" << std::endl;
		return false;
	}

	//initialize obj
	hr = model->Initialize(d3dHelperClass->GetDevice(), "Input link here", "something");
	if(!hr)
	{
		std::cerr << "Error! Could not initialize model!" << std::endl;
		return false;
	}

	//create window class






	return true;
}
