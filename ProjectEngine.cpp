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
	currentScore = -1;

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
	
	for (auto& mesh : objectEntities) 
		delete mesh;

	for (auto& entity : objectEntities)
		delete entity;

	for (auto& lightEntity : lightEntities)
		delete lightEntity;

	delete objectMaterial;
	delete planeMaterial;
	delete poolMaterial;
	delete waterMaterial;

	delete pixelShader;
	delete vertexShader;
	delete deferredPixelShader;
	delete deferredVertexShader;
	delete backBufferPixelShader;
	delete backBufferVertexShader;
	delete lightPixelShader;
	delete lightVertexShader;
	delete shadowVertexShader;
	delete dirPixelShader;
	delete dirVertexShader;
	delete particleVertexShader;
	delete particlePixelShader;
	delete waterVertexShader;
	delete waterPixelShader;
	
	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();
	sampler->Release();

	objectTextureSRV->Release();
	objectNormalMapSRV->Release();
	planeTextureSRV->Release();
	planeNormalMapSRV->Release();
	poolSRV->Release();
	poolNormalMapSRV->Release();
	waterNormalMapSRV->Release();

	depthStencilBuffer->Release();
	depthStencilView->Release();
	deferredRasterState->Release();
	blendState->Release();

	for (int i = 0; i < 3; i++)
	{
		renderTargetViewArray[i]->Release();
		shaderResourceViewArray[i]->Release();
	}
}


void Engine::Init()
{
	SetDeferred(width, height);
	CreateMatrices();
	LoadShaders();
	LoadModels();
	CreateWater();
	LoadTextures();	
	CreateMaterials();
	CreateEntities();
	LightEntityInitialize();
	// CreateShadow();

	// Tessellation - Displacement mapping
	tessellationAmount = 10.0f;
	HBias = 1.0f;
	HScale = 0.5f;
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::SetDeferred(int textureWidth, int textureHeight)
{
	int i;
	// Texture Discriptions
	D3D11_TEXTURE2D_DESC textureDescPosNorm;
	D3D11_TEXTURE2D_DESC textureDescDiffuse;

	// Texture for Position and Normal are 32 bit 
	textureDescPosNorm.Width = width;
	textureDescPosNorm.Height = height;
	textureDescPosNorm.MipLevels = 1;
	textureDescPosNorm.ArraySize = 1;
	textureDescPosNorm.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDescPosNorm.SampleDesc.Count = 1;
	textureDescPosNorm.SampleDesc.Quality = 0;
	textureDescPosNorm.Usage = D3D11_USAGE_DEFAULT;
	textureDescPosNorm.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescPosNorm.CPUAccessFlags = 0;
	textureDescPosNorm.MiscFlags = 0;

	//Texture for diffuse is 8 bit 
	textureDescDiffuse.Width = width;
	textureDescDiffuse.Height = height;
	textureDescDiffuse.MipLevels = 1;
	textureDescDiffuse.ArraySize = 1;
	textureDescDiffuse.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescDiffuse.SampleDesc.Count = 1;
	textureDescDiffuse.SampleDesc.Quality = 0;
	textureDescDiffuse.Usage = D3D11_USAGE_DEFAULT;
	textureDescDiffuse.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescDiffuse.CPUAccessFlags = 0;
	textureDescDiffuse.MiscFlags = 0;

	//Create renderer target array
	ID3D11Texture2D* renderTargetTextureArray[3];

	device->CreateTexture2D(&textureDescPosNorm, 0, &renderTargetTextureArray[0]);
	device->CreateTexture2D(&textureDescPosNorm, 0, &renderTargetTextureArray[1]);
	device->CreateTexture2D(&textureDescPosNorm, 0, &renderTargetTextureArray[2]);

	// renderer target desc for 32bit
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc32;
	ZeroMemory(&renderTargetViewDesc32, sizeof(renderTargetViewDesc32));
	renderTargetViewDesc32.Format = textureDescPosNorm.Format;
	renderTargetViewDesc32.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc32.Texture2D.MipSlice = 0;

	// renderer target desc for 8bit
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc8;
	ZeroMemory(&renderTargetViewDesc8, sizeof(renderTargetViewDesc8));
	renderTargetViewDesc8.Format = textureDescPosNorm.Format;
	renderTargetViewDesc8.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc8.Texture2D.MipSlice = 0;

	//Create the renderer target arrays
	device->CreateRenderTargetView(renderTargetTextureArray[0], &renderTargetViewDesc32, &renderTargetViewArray[0]);
	device->CreateRenderTargetView(renderTargetTextureArray[1], &renderTargetViewDesc32, &renderTargetViewArray[1]);
	device->CreateRenderTargetView(renderTargetTextureArray[2], &renderTargetViewDesc8, &renderTargetViewArray[2]);

	//Creste SRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescDR;
	ZeroMemory(&shaderResourceViewDescDR, sizeof(shaderResourceViewDescDR));
	shaderResourceViewDescDR.Format = textureDescPosNorm.Format;
	shaderResourceViewDescDR.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescDR.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescDR.Texture2D.MipLevels = 1;

	//Create SRV's
	for (i = 0; i < 3; i++)
	{
		device->CreateShaderResourceView(renderTargetTextureArray[i], &shaderResourceViewDescDR, &shaderResourceViewArray[i]);

	}

	//Release renderer target texture array
	for (i = 0; i < 3; i++)
	{
		renderTargetTextureArray[i]->Release();
	}

	//Create depth buffer desc
	D3D11_TEXTURE2D_DESC depthBufferDescDR;
	ZeroMemory(&depthBufferDescDR, sizeof(depthBufferDescDR));
	depthBufferDescDR.Width = width;
	depthBufferDescDR.Height = height;
	depthBufferDescDR.MipLevels = 1;
	depthBufferDescDR.ArraySize = 1;
	//depthBufferDescDR.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescDR.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDescDR.SampleDesc.Count = 1;
	depthBufferDescDR.SampleDesc.Quality = 0;
	depthBufferDescDR.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescDR.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDescDR.CPUAccessFlags = 0;
	depthBufferDescDR.MiscFlags = 0;

	device->CreateTexture2D(&depthBufferDescDR, 0, &depthStencilBuffer);

	//Create depth stencil desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescDR;
	ZeroMemory(&depthStencilViewDescDR, sizeof(depthStencilViewDescDR));
	depthStencilViewDescDR.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescDR.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescDR.Texture2D.MipSlice = 0;

	//Create depth stencil view
	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDescDR, &depthStencilView);


	//Setup the viewport for rendering.
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//context->OMSetRenderTargets(3, renderTargetViewArray, depthStencilViewDR);

	//Setup rasterizer state 
	D3D11_RASTERIZER_DESC rasterizerDescDR;
	ZeroMemory(&rasterizerDescDR, sizeof(rasterizerDescDR));
	rasterizerDescDR.CullMode = D3D11_CULL_NONE;
	rasterizerDescDR.FillMode = D3D11_FILL_SOLID;
	rasterizerDescDR.DepthClipEnable = false;

	device->CreateRasterizerState(&rasterizerDescDR, &deferredRasterState);

	//Setup blend state 
	D3D11_BLEND_DESC blendDescDR;
	ZeroMemory(&blendDescDR, sizeof(blendDescDR));
	blendDescDR.AlphaToCoverageEnable = false;
	blendDescDR.IndependentBlendEnable = false;
	blendDescDR.RenderTarget[0].BlendEnable = true;
	blendDescDR.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescDR.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescDR.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDescDR, &blendState);

	//TODO : DEPTH STENCIL STATE
	//TODO : Set the OMdepthstate also change depth target in Set renderer target

	D3D11_DEPTH_STENCIL_DESC depthStencilDescDR = {};
	depthStencilDescDR.DepthEnable = true;
	depthStencilDescDR.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDescDR.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDescDR, &depthState);
}

void Engine::CreateMatrices()
{
	// Camera
	camera = new Camera(0, 1, -15);
	camera->UpdateProjectionMatrix((float)width / height);

	// Shadow view matrix (basicly the light's perspective)
	XMMATRIX shadowView = XMMatrixLookAtLH(
		XMVectorSet(0, 0, -10, 0), // Eye position
		XMVectorSet(0, 0, 0, 0),		// Look at pos
		XMVectorSet(0, 1, 0, 0));		// Up
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shadowView));

	// Shadow projection matrix
	XMMATRIX shadowProj = XMMatrixOrthographicLH(
		10.0f,		// Ortho width
		10.0f,		// Ortho height       NOTE: tweak these values???
		0.1f,		// Near plane
		100.0f);	// Far plane
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shadowProj));
}

void Engine::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");		

	pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	deferredVertexShader = new SimpleVertexShader(device, context);
	if (!deferredVertexShader->LoadShaderFile(L"Debug/DeferredVS.cso"))
		deferredVertexShader->LoadShaderFile(L"DeferredVS.cso");

	deferredPixelShader = new SimplePixelShader(device, context);
	if (!deferredPixelShader->LoadShaderFile(L"Debug/DeferredPS.cso"))
		deferredPixelShader->LoadShaderFile(L"DeferredPS.cso");

	backBufferVertexShader = new SimpleVertexShader(device, context);
	if (!backBufferVertexShader->LoadShaderFile(L"Debug/BackBufferVS.cso"))
		backBufferVertexShader->LoadShaderFile(L"BackBufferVS.cso");

	backBufferPixelShader = new SimplePixelShader(device, context);
	if (!backBufferPixelShader->LoadShaderFile(L"Debug/BackBufferPS.cso"))
		backBufferPixelShader->LoadShaderFile(L"BackBufferPS.cso");

	lightVertexShader = new SimpleVertexShader(device, context);
	if (!lightVertexShader->LoadShaderFile(L"Debug/LightVS.cso"))
		lightVertexShader->LoadShaderFile(L"LightVS.cso");

	lightPixelShader = new SimplePixelShader(device, context);
	if (!lightPixelShader->LoadShaderFile(L"Debug/LightPS.cso"))
		lightPixelShader->LoadShaderFile(L"LightPS.cso");

	shadowVertexShader = new SimpleVertexShader(device, context);
	if (!shadowVertexShader->LoadShaderFile(L"Debug/ShadowVS.cso"))
		shadowVertexShader->LoadShaderFile(L"ShadowVS.cso");

	dirVertexShader = new SimpleVertexShader(device, context);
	if (!dirVertexShader->LoadShaderFile(L"Debug/DirectionalVS.cso"))
		dirVertexShader->LoadShaderFile(L"DirectionalVS.cso");

	dirPixelShader = new SimplePixelShader(device, context);
	if (!dirPixelShader->LoadShaderFile(L"Debug/DirectionalPS.cso"))
		dirPixelShader->LoadShaderFile(L"DirectionalPS.cso");

	particleVertexShader = new SimpleVertexShader(device, context);
	if (!particleVertexShader->LoadShaderFile(L"Debug/ParticleVS.cso"))
		particleVertexShader->LoadShaderFile(L"ParticleVS.cso");

	particlePixelShader = new SimplePixelShader(device, context);
	if (!particlePixelShader->LoadShaderFile(L"Debug/ParticlePS.cso"))
		particlePixelShader->LoadShaderFile(L"ParticlePS.cso");

	waterVertexShader = new SimpleVertexShader(device, context);
	if (!waterVertexShader->LoadShaderFile(L"Debug/WaterVS.cso"))
		waterVertexShader->LoadShaderFile(L"WaterVS.cso");

	waterPixelShader = new SimplePixelShader(device, context);
	if (!waterPixelShader->LoadShaderFile(L"Debug/waterPixelShader.cso"))
		waterPixelShader->LoadShaderFile(L"waterPixelShader.cso");

	backfaceGeometryShader = new SimpleGeometryShader(device, context);
	if (!backfaceGeometryShader->LoadShaderFile(L"Debug/BackfaceGS.cso"))
		backfaceGeometryShader->LoadShaderFile(L"BackfaceGS.cso");

	displacementDomainShader = new SimpleDomainShader(device, context);
	if (!displacementDomainShader->LoadShaderFile(L"Debug/DisplacementDS.cso"))
		displacementDomainShader->LoadShaderFile(L"DisplacementDS.cso");

	displacementHullShader = new SimpleHullShader(device, context);
	if (!displacementHullShader->LoadShaderFile(L"Debug/DisplacementHS.cso"))
		displacementHullShader->LoadShaderFile(L"DisplacementHS.cso");
}

void Engine::LoadModels()
{
	objectMesh = new Mesh(device, "Models/monkey.obj");
	ringMesh = new Mesh(device, "Models/torusRing.obj");
	planeMesh = new Mesh(device, "Models/plane.obj");
	lightMesh = new Mesh(device, "Models/sphere.obj");
	poolMesh = new Mesh(device, "Models/pool.obj");
	waterMesh = new Mesh(device, "Models/plane.obj");
}

void Engine::CreateWater()
{
	water = new Water();
	water->Initialize(device, L"Textures/NormalMaps/waterNormals.jpg", 3.0f, 1.0f);


}

void Engine::LoadTextures()
{
	// Load texture stuff.
	// OBJECTS
	// SPHERE
	dx::CreateWICTextureFromFile(device, context, L"Textures/seamlessMetal.jpg", 0, &objectTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Textures/NormalMaps/metalNormals.jpg", 0, &objectNormalMapSRV);

	// PLANE / GROUND
	dx::CreateWICTextureFromFile(device, context, L"Textures/seamlessSnow.jpg", 0, &planeTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Textures/NormalMaps/snowNormals.jpg", 0, &planeNormalMapSRV);

	// POOL AND WATER
	dx::CreateWICTextureFromFile(device, context, L"Textures/bathTiles.jpg", 0, &poolSRV);
	dx::CreateWICTextureFromFile(device, context, L"Textures/NormalMaps/triangleTileNormals.jpg", 0, &poolNormalMapSRV);

	dx::CreateWICTextureFromFile(device, context, L"Textures/coollink.png", 0, &waterSRV);
	dx::CreateWICTextureFromFile(device, context, L"Textures/NormalMaps/linkNormal.jpg", 0, &waterNormalMapSRV);
}

void Engine::CreateMaterials()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);

	objectMaterial = new Material(vertexShader, pixelShader, objectTextureSRV, objectNormalMapSRV, sampler);
	ringMaterial = new Material(vertexShader, pixelShader, objectTextureSRV, objectNormalMapSRV, sampler);
	planeMaterial = new Material(vertexShader, pixelShader, planeTextureSRV, planeNormalMapSRV, sampler);
	poolMaterial = new Material(vertexShader, pixelShader, poolSRV, poolNormalMapSRV, sampler);
	waterMaterial = new Material(waterVertexShader, waterPixelShader, waterSRV, waterNormalMapSRV, sampler);
}

void Engine::CreateEntities()
{
	// Create a plane (ground).
	EngineEntity* ground = new EngineEntity(planeMesh, planeMaterial);

	// Create objects.
	EngineEntity* monkey = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* monkey1 = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* monkey2 = new EngineEntity(objectMesh, objectMaterial);

	EngineEntity* ring = new EngineEntity(ringMesh, ringMaterial);

	// Pool and water
	EngineEntity* pool = new EngineEntity(poolMesh, poolMaterial);
	EngineEntity* water = new EngineEntity(waterMesh, waterMaterial);

	// Add plane (ground) to vector.
	objectEntities.push_back(ground);

	// Add objects to vector.
	objectEntities.push_back(monkey);
	objectEntities.push_back(monkey1);
	objectEntities.push_back(monkey2);
	objectEntities.push_back(ring);

	// Add pool and water to vector.
	objectEntities.push_back(pool);
	objectEntities.push_back(water);

	monkey->SetScale(1.0f, 1.0f, 1.0f);
	monkey->SetPosition(0.0f, 1.0f, 0.0f);

	monkey1->SetScale(1.0f, 1.0f, 1.0f);
	monkey1->SetPosition(4.0f, 1.0f, 0.0f);

	monkey2->SetScale(1.0f, 1.0f, 1.0f);
	monkey2->SetPosition(-4.0f, 1.0f, 0.0f);

	ring->SetScale(1.0f, 1.0f, 1.0f);
	ring->SetPosition(0.0f, 3.5f, 0.0f);
	ring->SetRotation(1.1f, 0.0f, 0.0f);
	
	// Bottom
	ground->SetScale(10.0f, 0.025f, 10.0f);
	ground->SetPosition(0.0f, -2.0f, 0.0f);

	pool->SetScale(1.0f, 1.0f, 1.0f);
	pool->SetPosition(0.0f, -1.9f, 0.0f);

	water->SetScale(3.0f, 0.05f, 1.8f);
	water->SetPosition(0.0f, -1.8f, 0.0f);
}

void Engine::LightEntityInitialize()
{
	// TODO: Make the scene brighter.
	lightEntity1 = new EngineEntity(lightMesh, dx::XMFLOAT3(1.0f, 0.0f, 0.0f));
	lightEntity1->SetPosition(-4, 1, 0);
	lightEntity1->SetScale(10, 10, 10);

	lightEntity2 = new EngineEntity(lightMesh, dx::XMFLOAT3(0.0f, 1.0f, 0.0f));
	lightEntity2->SetPosition(0, 1, 0);
	lightEntity2->SetScale(10, 10, 10);

	lightEntity3 = new EngineEntity(lightMesh, dx::XMFLOAT3(0.0f, 0.0f, 1.0f));
	lightEntity3->SetPosition(4, 1, 0);
	lightEntity3->SetScale(10, 10, 10);

	lightEntity4 = new EngineEntity(lightMesh, dx::XMFLOAT3(1.0f, 0.6f, 0.2f));
	lightEntity4->SetPosition(-3, 0, 2);
	lightEntity4->SetScale(5, 5, 5);

	lightEntity5 = new EngineEntity(lightMesh, dx::XMFLOAT3(0.2f, 0.6f, 0.8f));
	lightEntity5->SetPosition(3, 0, -2);
	lightEntity5->SetScale(5, 5, 5);

	lightEntity6 = new EngineEntity(lightMesh, dx::XMFLOAT3(0.2f, 0.6f, 0.8f));
	lightEntity6->SetPosition(-3, 0, -2);
	lightEntity6->SetScale(5, 5, 5);

	lightEntity7 = new EngineEntity(lightMesh, dx::XMFLOAT3(0.2f, 0.8f, 0.2f));
	lightEntity7->SetPosition(3, 0, 1);
	lightEntity7->SetScale(5, 5, 5);

	lightEntity8 = new EngineEntity(lightMesh, dx::XMFLOAT3(0.2f, 0.8f, 0.2f));
	lightEntity8->SetPosition(-3, 0, 1);
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

//void Engine::CreateShadows()
//{
//	shadowMapSize = 2048;  // Correct value?
//
//	// Create the texture that will be used as shadow map
//	D3D11_TEXTURE2D_DESC shadowDescription = {};
//	shadowDescription.Width = shadowMapSize;
//	shadowDescription.Height = shadowMapSize;
//	shadowDescription.ArraySize = 1;
//	shadowDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
//	shadowDescription.CPUAccessFlags = 0;
//	shadowDescription.Format = DXGI_FORMAT_R32_TYPELESS;
//	shadowDescription.MipLevels = 1;
//	shadowDescription.MiscFlags = 0;
//	shadowDescription.SampleDesc.Count = 1;
//	shadowDescription.SampleDesc.Quality = 0;
//	shadowDescription.Usage = D3D11_USAGE_DEFAULT;
//	ID3D11Texture2D* shadowTexture;
//	device->CreateTexture2D(&shadowDescription, 0, &shadowTexture);
//
//	// Create depth/stencil
//	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
//	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
//	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	shadowDSDesc.Texture2D.MipSlice = 0;
//	//device->CreateDepthStencilView(shadowTexture, &shadowDescription, &shadowDSV);
//
//	// Create SRV for shadow map
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDescrition = {};
//	srvDescrition.Format = DXGI_FORMAT_R32_FLOAT;
//	srvDescrition.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDescrition.Texture2D.MipLevels = 1;
//	srvDescrition.Texture2D.MostDetailedMip = 0;
//	device->CreateShaderResourceView(shadowTexture, &srvDescrition, &shadowSRV);
//
//	// Now we don't need the texture anymore
//	shadowTexture->Release();
//
//	// Create the special "comparison" sampler state for our shadows
//	D3D11_SAMPLER_DESC shadowSampDescription = {};
//	shadowSampDescription.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
//	shadowSampDescription.ComparisonFunc = D3D11_COMPARISON_LESS;
//	shadowSampDescription.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//	shadowSampDescription.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//	shadowSampDescription.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//	shadowSampDescription.BorderColor[0] = 1.0f;
//	shadowSampDescription.BorderColor[1] = 1.0f;
//	shadowSampDescription.BorderColor[2] = 1.0f;
//	shadowSampDescription.BorderColor[3] = 1.0f;
//	device->CreateSamplerState(&shadowSampDescription, &shadowSampler);
//
//	// Create rasterizer state
//	D3D11_RASTERIZER_DESC shadowRastDescription = {};
//	shadowRastDescription.FillMode = D3D11_FILL_SOLID;
//	shadowRastDescription.CullMode = D3D11_CULL_BACK;
//	shadowRastDescription.DepthClipEnable = true;
//	shadowRastDescription.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
//	shadowRastDescription.DepthBiasClamp = 0.0f;
//	shadowRastDescription.SlopeScaledDepthBias = 1.0f;
//	device->CreateRasterizerState(&shadowRastDescription, &shadowRasterizer);
//
//	//Blending State
//	//Set up the Blend Description
//	D3D11_BLEND_DESC blendDescription;
//	ZeroMemory(&blendDescription, sizeof(blendDescription));
//	blendDescription.RenderTarget[0].BlendEnable = TRUE;
//	blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//	blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
//	blendDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
//	blendDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//	//Setting the Blend Description to FALSE (will be be used in OMBlendState in Draw function)
//	blendDescription.RenderTarget[0].BlendEnable = FALSE;
//	device->CreateBlendState(&blendDescription, &blendState);
//
//}

//void Engine::RenderShadowMap()
//{
//	// Initial setup
//	context->OMSetRenderTargets(0, 0, shadowDSV);
//	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
//	context->RSSetState(shadowRasterizer);
//
//	// Set up shadow view port    
//	D3D11_VIEWPORT shadowViewPort = {};
//	shadowViewPort.TopLeftX = 0;
//	shadowViewPort.TopLeftY = 0;
//	shadowViewPort.Width = (float)shadowMapSize;
//	shadowViewPort.Height = (float)shadowMapSize;
//	shadowViewPort.MinDepth = 0.0f;
//	shadowViewPort.MaxDepth = 1.0f;
//	context->RSSetViewports(1, &shadowViewPort);
//
//	// Set up shaders for making the shadow map
//	shadowVertexShader->SetShader();
//	shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix);
//	shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);
//
//	// Turn off the pixel shader, we don´t need that stuff
//	context->PSSetShader(0, 0, 0);
//
//	// Actually draw our entities
//	UINT stride = sizeof(Vertex);
//	UINT offset = 0;
//
//	for (unsigned int i = 0; i < entities.size(); i++)
//	{
//
//		// Grab the data from the first entity's mesh
//		EngineEntity* entity =  entities[i];
//		ID3D11Buffer* vertexBuffer = entity->GetMesh()->GetVertexBuffer();
//		ID3D11Buffer* indexBuffer = entity->GetMesh()->GetIndexBuffer();
//
//		// Set buffers in the input assembler
//		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
//		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//		shadowVertexShader->SetMatrix4x4("world", entity->GetWorldMatrix());
//		shadowVertexShader->CopyAllBufferData();
//
//		// Finally do the actual drawing
//		context->DrawIndexed(entity->GetMesh()->GetIndexCount(), 0, 0);
//	}
//
//	// Revert to original targets and states
//	//context->OMSetRenderTargets(1, &this->backBufferRTV, this->depthStencilView);
//	context->OMSetRenderTargets(1, &this->renderTargetViewArray[BUFFER_COUNT], this->depthStencilView);
//	shadowViewPort.Width = (float)this->width;
//	shadowViewPort.Height = (float)this->height;
//	context->RSSetViewports(1, &shadowViewPort);
//	context->RSSetState(0);
//}

void Engine::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);
	water->Update();

	for (unsigned int i = 0; i < objectEntities.size(); i++)
	{
		objectEntities[i]->UpdateWorldMatrix();
	}

	// waterEntity[0]->UpdateWorldMatrix();
	
	for (unsigned int j = 0; j < lightEntities.size(); j++)
	{
		lightEntities[j]->UpdateWorldMatrix();
	}


	// Logic for flying through the torus ring object as a checkpoint. The checkpoint gives a point
	// every time the player flies through it.
	
	// Check if player went through ring
	if(currentScore == -1)
	{
		currentScore++;
		std::cout << "Welcome to the game! Your goal is to go through the ring 10 times to win! \nYour current score is: " << currentScore << std::endl;
	}
	
	// Print camera position.
	// std::cout << "x: " << camera->GetPosition().x << "\ny: " << camera->GetPosition().y << "\nz: " << camera->GetPosition().z << std::endl;

	// Access ring's position. Has index 4 in entities vector.
	if(camera->GetPosition().x > (objectEntities[4]->GetPosition().x - 0.5f) && camera->GetPosition().x < (objectEntities[4]->GetPosition().x + 0.5f))
	{
		if(camera->GetPosition().y > (objectEntities[4]->GetPosition().y - 1.0f) && camera->GetPosition().y < objectEntities[4]->GetPosition().y + 1.0f)
		{
			if (camera->GetPosition().z > objectEntities[4]->GetPosition().z - 0.5f && camera->GetPosition().z < objectEntities[4]->GetPosition().z + 0.5f )
			{
																							// MAX - MIN
				float randomX = -5.0f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(10.0f)));
				float randomY = 0.5f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(4.0f)));
				float randomZ = -5.0f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(10.0f)));
				while(randomZ > -1.0f && randomZ < 1.0f) // So ring doesn't end up inside monkees.
				{
					randomZ = -5.0f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(10.0f)));
				}
				objectEntities[4]->SetPosition(randomX, randomY, randomZ);
				camera->ResetCamera();
				currentScore++;
				std::cout << "Your current score is: " << currentScore << std::endl;
				std::cout << "The ring's position is: " << objectEntities[4]->GetPosition().x << "  " 
					<< objectEntities[4]->GetPosition().y << "  " << objectEntities[4]->GetPosition().z << std::endl;

				// Win condition, reset position of ring to center and reset score.
				if(currentScore == 10)
				{
					objectEntities[4]->SetPosition(0.0f, 3.5f, 0.0f);
					std::cout << "You win!" << std::endl;
					currentScore = 0;
					std::cout << "Why not go again?" "\nYour current score is: " << currentScore << std::endl;
				}
			}
		}
	}

}

void Engine::Draw(float deltaTime, float totalTime)
{
	// Background color for clearing
	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->OMSetRenderTargets(3, renderTargetViewArray, depthStencilView);
	context->RSSetViewports(1, &viewport);

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		context->ClearRenderTargetView(renderTargetViewArray[i], color);
	}

	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Backface culling. KOLLA IN DETTAAAAA
	/*backfaceGeometryShader->SetFloat3("camPos", camera->GetPosition());
	backfaceGeometryShader->CopyAllBufferData();
	backfaceGeometryShader->SetShader();*/

	// context->GSSetShader(backfaceGeometryShader, nullptr, 0);

	// Geometry pass.

	for (int i = 0; i < objectEntities.size(); i++)
	{
		renderer.RenderGBuffer(objectEntities[i], vertexBuffer, indexBuffer, deferredVertexShader, deferredPixelShader, camera, context);
	}

	// renderer.RenderWater(objectEntities[6], vertexBuffer, indexBuffer, waterVertexShader, waterPixelShader, camera, context);

	//-----------------------------
	/*
		context->OMSetRenderTargets(1, &backBufferRTV, 0);
		context->RSSetViewports(1, &viewport);

		context->ClearRenderTargetView(backBufferRTV, color);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		displayVertexShader->SetShader();

		switch (switcher)
		{
		case 1:
			displayPixelShader->SetShaderResourceView("Texture", shaderResourceViewArray[0]);
			break;
		case 2:
			displayPixelShader->SetShaderResourceView("Texture", shaderResourceViewArray[1]);
			break;
		case 3:
			displayPixelShader->SetShaderResourceView("Texture", shaderResourceViewArray[2]);
			break;
		//case 4:
			//displayPixelShader->SetShaderResourceView("Texture", depthSRV);
			//break;
		default:
			break;
		}

		ID3D11Buffer* nothing = 0;
		context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

		// Actually draw exactly 3 vertices
		context->Draw(3, 0);
		displayPixelShader->SetShaderResourceView("Texture", 0);
		/**/
		//-------------------	

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->RSSetViewports(1, &viewport);

	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	context->RSSetState(deferredRasterState);
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(blendState, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthState, 0);

	//---------------- Directional Light pass ----------------
	dirVertexShader->SetShader();

	dirPixelShader->SetShaderResourceView("positionGB", shaderResourceViewArray[0]);
	dirPixelShader->SetShaderResourceView("normalGB", shaderResourceViewArray[1]);
	dirPixelShader->SetShaderResourceView("diffuseGB", shaderResourceViewArray[2]);
	backBufferPixelShader->SetSamplerState("Sampler", sampler);

	dirPixelShader->SetFloat3("lightColor", XMFLOAT3(0.4f, 0.4f, 0.7f));
	dirPixelShader->SetFloat3("lightDir", XMFLOAT3(0.0f, -1.0f, 0.0f));

	dirPixelShader->CopyAllBufferData();
	dirPixelShader->SetShader();

	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(3, 0);
	//-----------------

	// Light pass.
	for (int i = 0; i < lightEntities.size(); i++)
	{
		renderer.RenderLights(lightEntities[i], vertexBuffer, indexBuffer, lightVertexShader, lightPixelShader, 
			camera, context, sampler, shaderResourceViewArray[0], shaderResourceViewArray[1], shaderResourceViewArray[2]);
	}

	//---------------
	context->RSSetState(NULL);
	context->OMSetBlendState(NULL, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(NULL, 0);
	//--------------------------	
	swapChain->Present(0, 0);
}

// MOUSE INPUTS
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