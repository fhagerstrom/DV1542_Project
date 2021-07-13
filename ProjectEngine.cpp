#include "ProjectEngine.h"

// For the DirectX Math library
using namespace DirectX;


Engine::Engine(HINSTANCE hInstance) : DXInit( 
		hInstance,		   // The application's handle
		"3D Project",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats or not.
{
	// Initialize fields.
	vertexBuffer = 0;
	indexBuffer = 0;
	vertexShader = 0;
	pixelShader = 0;
	camera = 0;

	for (int i = 0; i < 3; i++)
	{
		renderTargetViewArray[i] = 0;
		shaderResourceViewArray[i] = 0;
	}

	depthStencilBuffer = 0;
	depthStencilView = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Create a console window 
	CreateConsoleWindow(500, 120, 32, 120);
	std::cout << "Console window created successfully. Prints for debugging goes here!" << std::endl;;
#endif
}


Engine::~Engine()
{
	delete camera;

	delete pixelShader;
	delete vertexShader;

	for (auto& mesh : meshes) 
		delete mesh;

	delete objectMaterial;
	delete planeMaterial;

	for (auto& entity : entities) 
		delete entity;

	for (auto& lightEntity : lightEntities) 
		delete lightEntity;

	sampler->Release();

	objectTextureSRV->Release();
	objectNormalMapSRV->Release();
	planeTextureSRV->Release();
	planeNormalMapSRV->Release();

	depthStencilBuffer->Release();
	depthStencilView->Release();
	deferredRasterState->Release();
	blendState->Release();


	for (int i = 0; i < 3; i++)
	{
		renderTargetViewArray[i]->Release();
		shaderResourceViewArray[i]->Release();
	}
	
	delete deferredPixelShader;
	delete deferredVertexShader;
	delete backBufferPixelShader;
	delete backBufferVertexShader;
	delete lightPixelShader;
	delete lightVertexShader;
	delete dirPixelShader;
	delete dirVertexShader;
}


void Engine::Init()
{
	SetDefferedSetup(width, height);
	CreateMatrices();
	LoadShaders();
	LoadMesh();
	LoadTextures();	
	CreateMaterials();
	CreateBasicGeometry();	
	LightEntityInitialize();
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::SetDefferedSetup(int textureWidth, int textureHeight)
{
	int i;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* renderTargetTextureArray[BUFFER_COUNT];

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		device->CreateTexture2D(&textureDesc, NULL, &renderTargetTextureArray[i]);

	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc32;
	ZeroMemory(&renderTargetViewDesc32, sizeof(renderTargetViewDesc32));
	renderTargetViewDesc32.Format = textureDesc.Format;
	renderTargetViewDesc32.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc32.Texture2D.MipSlice = 0;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc8;
	ZeroMemory(&renderTargetViewDesc8, sizeof(renderTargetViewDesc8));
	renderTargetViewDesc8.Format = textureDesc.Format;
	renderTargetViewDesc8.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc8.Texture2D.MipSlice = 0;

	// Create Render target view
	device->CreateRenderTargetView(renderTargetTextureArray[0], &renderTargetViewDesc32, &renderTargetViewArray[0]);
	device->CreateRenderTargetView(renderTargetTextureArray[1], &renderTargetViewDesc32, &renderTargetViewArray[1]);
	device->CreateRenderTargetView(renderTargetTextureArray[2], &renderTargetViewDesc8, &renderTargetViewArray[2]);

	// Shader Resource View Description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		device->CreateShaderResourceView(renderTargetTextureArray[i], &srvDesc, &shaderResourceViewArray[i]);
	}

	// Release render target texture array
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		renderTargetTextureArray[i]->Release();
	}

	// Depth Buffer Description
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

	// Depth Stencil Description
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);

	// Rasterizer setup
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = false;

	device->CreateRasterizerState(&rasterizerDesc, &deferredRasterState);

	// Blend state setup
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDesc, &blendState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthStencilDesc, &depthState);
}

void Engine::CreateMatrices()
{
	camera = new Camera(0, 0, -5);
	camera->UpdateProjectionMatrix((float)width / height);
}

void Engine::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"Debug/VertexShader.cso");		

	pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"Debug/PixelShader.cso");

	deferredVertexShader = new SimpleVertexShader(device, context);
	if (!deferredVertexShader->LoadShaderFile(L"Debug/DeferredVertexShader.cso"))
		deferredVertexShader->LoadShaderFile(L"Debug/DeferredVertexShader.cso");

	deferredPixelShader = new SimplePixelShader(device, context);
	if (!deferredPixelShader->LoadShaderFile(L"Debug/DeferredPixelShader.cso"))
		deferredPixelShader->LoadShaderFile(L"Debug/DeferredPixelShader.cso");

	backBufferVertexShader = new SimpleVertexShader(device, context);
	if (!backBufferVertexShader->LoadShaderFile(L"Debug/BackBufferVertexShader.cso"))
		backBufferVertexShader->LoadShaderFile(L"Debug/BackBufferVertexShader.cso");

	backBufferPixelShader = new SimplePixelShader(device, context);
	if (!backBufferPixelShader->LoadShaderFile(L"Debug/BackBufferPixelShader.cso"))
		backBufferPixelShader->LoadShaderFile(L"Debug/BackBufferPixelShader.cso");

	lightVertexShader = new SimpleVertexShader(device, context);
	if (!lightVertexShader->LoadShaderFile(L"Debug/LightVertexShader.cso"))
		lightVertexShader->LoadShaderFile(L"Debug/LightVertexShader.cso");

	lightPixelShader = new SimplePixelShader(device, context);
	if (!lightPixelShader->LoadShaderFile(L"Debug/LightPixelShader.cso"))
		lightPixelShader->LoadShaderFile(L"Debug/LightPixelShader.cso");

	dirVertexShader = new SimpleVertexShader(device, context);
	if (!dirVertexShader->LoadShaderFile(L"Debug/DirectionalVertexShader.cso"))
		dirVertexShader->LoadShaderFile(L"Debug/DirectionalVertexShader.cso");

	dirPixelShader = new SimplePixelShader(device, context);
	if (!dirPixelShader->LoadShaderFile(L"Debug/DirectionalPixelShader.cso"))
		dirPixelShader->LoadShaderFile(L"Debug/DirectionalPixelShader.cso");

	particleVertexShader = new SimpleVertexShader(device, context);
	if (!particleVertexShader->LoadShaderFile(L"Debug/ParticleVertexShader.cso"))
		particleVertexShader->LoadShaderFile(L"Debug/ParticleVertexShader.cso");

	particlePixelShader = new SimplePixelShader(device, context);
	if (!particlePixelShader->LoadShaderFile(L"Debug/ParticlePixelShader.cso"))
		particlePixelShader->LoadShaderFile(L"Debug/ParticlePixelShader.cso");

	waterVertexShader = new SimpleVertexShader(device, context);
	if (!particleVertexShader->LoadShaderFile(L"Debug/WaterVertexShader.cso"))
		particleVertexShader->LoadShaderFile(L"Debug/WaterVertexShader.cso");

	waterPixelShader = new SimplePixelShader(device, context);
	if (!particlePixelShader->LoadShaderFile(L"Debug/WaterPixelShader.cso"))
		particlePixelShader->LoadShaderFile(L"Debug/WaterPixelShader.cso");
}

void Engine::LoadMesh()
{
	objectMesh = new Mesh(device, "Debug/Models/sphere.obj");
	planeMesh = new Mesh(device, "Debug/Models/cube.obj");
}

void Engine::LoadTextures()
{
	// Load texture stuff
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/scrapedMetal.jpg", 0, &objectTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/metalNormals.jpg", 0, &objectNormalMapSRV);

	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/crackedIce.jpg", 0, &planeTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/iceNormals.png", 0, &planeNormalMapSRV);
}

void Engine::CreateMaterials()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);

	objectMaterial = new Material(vertexShader, pixelShader, objectTextureSRV, objectNormalMapSRV, sampler);
	planeMaterial = new Material(vertexShader, pixelShader, planeTextureSRV, planeNormalMapSRV, sampler);
}

void Engine::CreateBasicGeometry()
{
	// Create objects and planes.
	EngineEntity* sphere = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* sphere1 = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* sphere2 = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* sphere3 = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* sphere4 = new EngineEntity(objectMesh, objectMaterial);
	
	EngineEntity* plane1 = new EngineEntity(planeMesh, planeMaterial);
	EngineEntity* plane2 = new EngineEntity(planeMesh, planeMaterial);
	EngineEntity* plane3 = new EngineEntity(planeMesh, planeMaterial);
	EngineEntity* plane4 = new EngineEntity(planeMesh, planeMaterial);

	// Add objects to vector.
	entities.push_back(sphere);
	entities.push_back(sphere1);
	entities.push_back(sphere2);
	entities.push_back(sphere3);
	entities.push_back(sphere4);

	// Add planes to vector.
	entities.push_back(plane1);
	entities.push_back(plane2);
	entities.push_back(plane3);
	entities.push_back(plane4);

	sphere->SetScale(0.5f, 0.5f, 0.5f);
	sphere->SetPosition(0.0f, -2.0f, 0.0f);

	sphere1->SetScale(0.5f, 0.5f, 0.5f);
	sphere1->SetPosition(-2.0f, -2.0f, -2.0f);

	sphere2->SetScale(0.5f, 0.5f, 0.5f);
	sphere2->SetPosition(2.0f, -2.0f, -2.0f);

	sphere3->SetScale(0.5f, 0.5f, 0.5f);
	sphere3->SetPosition(2.0f, -2.0f, 2.0f);

	sphere4->SetScale(0.5f, 0.5f, 0.5f);
	sphere4->SetPosition(-2.0f, -2.0f, 2.0f);

	// Bottom
	plane1->SetScale(8.0f, 0.01f, 10.0f);
	plane1->SetPosition(0.0f, -3.0f, 0.0f);

	// Right
	plane2->SetScale(0.01f, 3.0f, 5.0f);
	plane2->SetPosition(5.0f, 0.0f, 0.0f);

	// Left
	plane3->SetScale(0.01f, 3.0f, 5.0f);
	plane3->SetPosition(-5.0f, 0.0f, 0.0f);

	// Back
	plane4->SetScale(5.0f, 3.0f, 0.01f);
	plane4->SetPosition(0.0f, 0.0f, 5.0f);	

	currentEntity = 0;
}

void Engine::LightEntityInitialize()
{
	// @TODO: Make the scene brighter
	// Some weird shit happens
	lightEntity1 = new EngineEntity(objectMesh, dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	lightEntity1->SetPosition(3, 0, 0);
	lightEntity1->SetScale(5, 5, 5);

	lightEntity2 = new EngineEntity(objectMesh, dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	lightEntity2->SetPosition(-3, 0, 0);
	lightEntity2->SetScale(5, 5, 5);

	lightEntity3 = new EngineEntity(objectMesh, dx::XMFLOAT3(0.3f, 0.8f, 0.1f));
	lightEntity3->SetPosition(-1, 1, 0);
	lightEntity3->SetScale(5, 5, 5);

	lightEntity4 = new EngineEntity(objectMesh, dx::XMFLOAT3(0.1f, 0.8f, 0.5f));
	lightEntity4->SetPosition(1, 1, 0);
	lightEntity4->SetScale(5, 5, 5);

	lightEntity5 = new EngineEntity(objectMesh, dx::XMFLOAT3(1.0f, 0.2f, 0.2f));
	lightEntity5->SetPosition(-1, -1, 0);
	lightEntity5->SetScale(5, 5, 5);

	lightEntity6 = new EngineEntity(objectMesh, dx::XMFLOAT3(0.5f, 0.5f, 0.5f));
	lightEntity6->SetPosition(1, -1, 0);
	lightEntity6->SetScale(5, 5, 5);

	lightEntity7 = new EngineEntity(objectMesh, dx::XMFLOAT3(0.2f, 0.2f, 1.0f));
	lightEntity7->SetPosition(-2, 0, 1);
	lightEntity7->SetScale(5, 5, 5);

	lightEntity8 = new EngineEntity(objectMesh, dx::XMFLOAT3(0.5f, 0.2f, 1.0f));
	lightEntity8->SetPosition(2, 0, 1);
	lightEntity8->SetScale(5, 5, 5);

	lightEntities.push_back(lightEntity1);
	lightEntities.push_back(lightEntity2);
	lightEntities.push_back(lightEntity3);
	lightEntities.push_back(lightEntity4);
	lightEntities.push_back(lightEntity5);
	lightEntities.push_back(lightEntity6);
	lightEntities.push_back(lightEntity7);
	lightEntities.push_back(lightEntity8);
}

void Engine::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		entities[i]->UpdateWorldMatrix();
	}
	
	lightEntity1->UpdateWorldMatrix();
	lightEntity2->UpdateWorldMatrix();
	lightEntity3->UpdateWorldMatrix();
	lightEntity4->UpdateWorldMatrix();
	lightEntity5->UpdateWorldMatrix();
	lightEntity6->UpdateWorldMatrix();
	lightEntity7->UpdateWorldMatrix();
	lightEntity8->UpdateWorldMatrix();
}

void Engine::Draw(float deltaTime, float totalTime)
{
	const float bgColour[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	int i;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind render target view array and depth stencil buffer to output render pipeline
	context->OMSetRenderTargets(BUFFER_COUNT, renderTargetViewArray, depthStencilView);

	// Clear the render target buffers
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		context->ClearRenderTargetView(renderTargetViewArray[i], bgColour);
	}

	// Clear the depth buffer
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	
	
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		ID3D11Buffer* vBuffer = entities[i]->GetMesh()->GetVertexBuffer();
		ID3D11Buffer* iBuffer = entities[i]->GetMesh()->GetIndexBuffer();

		deferredVertexShader->SetMatrix4x4("world", entities[i]->GetWorldMatrix());
		deferredVertexShader->SetMatrix4x4("view", camera->GetView());
		deferredVertexShader->SetMatrix4x4("projection", camera->GetProjection());

		deferredVertexShader->CopyAllBufferData();
		deferredVertexShader->SetShader();

		deferredPixelShader->SetSamplerState("Sampler", entities[i]->GetMaterial()->GetSamplerState());
		deferredPixelShader->SetShaderResourceView("Texture", entities[i]->GetMaterial()->GetTextureSRV());
		deferredPixelShader->SetShaderResourceView("NormalMap", entities[i]->GetMaterial()->GetNormalMapSRV());

		deferredPixelShader->CopyAllBufferData();
		deferredPixelShader->SetShader();

		context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
		context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
		
		context->DrawIndexed(entities[i]->GetMesh()->GetIndexCount(), 0, 0);
	}
	
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->ClearRenderTargetView(backBufferRTV, bgColour);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	context->RSSetState(deferredRasterState);
	float blend[4] = { 1, 1, 1, 1 };
	context->OMSetBlendState(blendState, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthState, 0);

	dirVertexShader->SetShader();

	dirPixelShader->SetShaderResourceView("Position", shaderResourceViewArray[0]);
	dirPixelShader->SetShaderResourceView("NormalMap", shaderResourceViewArray[1]);
	dirPixelShader->SetShaderResourceView("Texture", shaderResourceViewArray[2]);
	backBufferPixelShader->SetSamplerState("Sampler", sampler);

	dirPixelShader->SetFloat3("lightColour", dx::XMFLOAT3(0.5f, 0.5f, 0.5f));
	dirPixelShader->SetFloat3("lightDirection", dx::XMFLOAT3(10.0f, -10.0f, 0.0f));

	dirPixelShader->CopyAllBufferData();
	dirPixelShader->SetShader();

	particleVertexShader->SetShader();

	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(3, 0);

	for (int i = 0; i < lightEntities.size(); i++)
	{
		ID3D11Buffer* vBufferLight = lightEntities[i]->GetMesh()->GetVertexBuffer();
		ID3D11Buffer* iBufferLight = lightEntities[i]->GetMesh()->GetIndexBuffer();

		context->IASetVertexBuffers(0, 1, &vBufferLight, &stride, &offset);
		context->IASetIndexBuffer(iBufferLight, DXGI_FORMAT_R32_UINT, 0);

		lightVertexShader->SetMatrix4x4("world", lightEntities[i]->GetWorldMatrix());
		lightVertexShader->SetMatrix4x4("view", camera->GetView());
		lightVertexShader->SetMatrix4x4("projection", camera->GetProjection());

		lightVertexShader->CopyAllBufferData();
		lightVertexShader->SetShader();

		lightPixelShader->SetShaderResourceView("NormalMap", shaderResourceViewArray[0]);
		lightPixelShader->SetShaderResourceView("Texture", shaderResourceViewArray[1]);
		lightPixelShader->SetShaderResourceView("PositionTexture", shaderResourceViewArray[2]);

		lightPixelShader->SetSamplerState("basicSampler", sampler);

		lightPixelShader->SetFloat3("cameraPosition", camera->GetPosition());
		
		lightPixelShader->SetFloat3("lightColour", lightEntities[i]->GetLightColour());
		lightPixelShader->SetFloat3("lightPosition", lightEntities[i]->GetPosition());

		lightPixelShader->CopyAllBufferData();
		lightPixelShader->SetShader();

		context->DrawIndexed(lightEntities[i]->GetMesh()->GetIndexCount(), 0, 0);
	}
	
	context->RSSetState(NULL);
	context->OMSetBlendState(NULL, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(NULL, 0);

	swapChain->Present(0, 0);
}

// MOUSE INPUTS - *** BYTA TILL DX INPUT ***
#pragma region Mouse Input

void Engine::MousePressed(WPARAM buttonState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hWnd);
}

void Engine::MouseReleased(WPARAM buttonState, int x, int y)
{
	ReleaseCapture();
}

void Engine::MouseMoving(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001)
	{
		float xDiff = (x - prevMousePos.x) * 0.005f;
		float yDiff = (y - prevMousePos.y) * 0.005f;
		camera->Rotate(yDiff, xDiff);
	}

	prevMousePos.x = x;
	prevMousePos.y = y;
}

#pragma endregion