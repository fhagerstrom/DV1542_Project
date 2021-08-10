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
	
	for (auto& mesh : entities) 
		delete mesh;

	for (auto& entity : entities)
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
	LoadMesh();
	LoadTextures();	
	CreateMaterials();
	CreateWater();
	CreateBasicGeometry();
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
	// DEFERRED SETUP
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

	for (int i = 0; i < BUFFER_COUNT; i++)
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

	for (int j = 0; j < BUFFER_COUNT; j++)
	{
		device->CreateShaderResourceView(renderTargetTextureArray[j], &srvDesc, &shaderResourceViewArray[j]);
	}

	// Release render target texture array
	for (int k= 0; k < BUFFER_COUNT; k++)
	{
		renderTargetTextureArray[k]->Release();
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
	camera = new Camera(0, 0, -10);
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

void Engine::LoadMesh()
{
	objectMesh = new Mesh(device, "Debug/Models/monkey.obj");
	ringMesh = new Mesh(device, "Debug/Models/torusRing.obj");
	planeMesh = new Mesh(device, "Debug/Models/cube.obj");
	lightMesh = new Mesh(device, "Debug/Models/sphere.obj");
	poolMesh = new Mesh(device, "Debug/Models/pool.obj");
	waterMesh = new Mesh(device, "Debug/Models/cube.obj");
}

void Engine::LoadTextures()
{
	// Load texture stuff.
	// OBJECTS
	// SPHERE
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/seamlessMetal.jpg", 0, &objectTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/NormalMaps/metalNormals.jpg", 0, &objectNormalMapSRV);

	// PLANE / GROUND
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/seamlessSnow.jpg", 0, &planeTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/NormalMaps/snowNormals.jpg", 0, &planeNormalMapSRV);

	// POOL AND WATER (WIP)
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/bathTiles.jpg", 0, &poolSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/NormalMaps/triangleTileNormals.jpg", 0, &poolNormalMapSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/seamlessWater.png", 0, &waterSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/NormalMaps/waterNormals.jpg", 0, &waterNormalMapSRV);
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

void Engine::CreateWater()
{
	// Init water class and call functions to create.
	water = new Water();
	water->Initialize(device, L"Debug/Textures/seamlessWater.png", 2.75f, 1.0f);
}

void Engine::CreateBasicGeometry()
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
	entities.push_back(ground);

	// Add objects to vector.
	entities.push_back(monkey);
	entities.push_back(monkey1);
	entities.push_back(monkey2);
	entities.push_back(ring);

	// Add pool and water to vector
	entities.push_back(pool);
	entities.push_back(water);

	monkey->SetScale(1.0f, 1.0f, 1.0f);
	monkey->SetPosition(0.0f, 1.0f, 0.0f);

	monkey1->SetScale(1.0f, 1.0f, 1.0f);
	monkey1->SetPosition(4.0f, 1.0f, 0.0f);

	monkey2->SetScale(1.0f, 1.0f, 1.0f);
	monkey2->SetPosition(-4.0f, 1.0f, 0.0f);

	ring->SetScale(1.0f, 1.0f, 1.0f);
	ring->SetPosition(0.0f, 3.5f, 0.0f);
	ring->SetRotation(80.0f, 0.0f, 0.0f);
	
	// Bottom
	ground->SetScale(10.0f, 0.025f, 10.0f);
	ground->SetPosition(0.0f, -2.0f, 0.0f);

	pool->SetScale(1.0f, 1.0f, 1.0f);
	pool->SetPosition(0.0f, -1.9f, 0.0f);

	water->SetScale(3.0f, 0.05f, 1.8f);
	water->SetPosition(0.0f, -1.8f, 0.0f);

	currentEntity = 0;
}

void Engine::LightEntityInitialize()
{
	// TODO: Make the scene brighter.
	lightEntity1 = new EngineEntity(lightMesh, dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	lightEntity1->SetPosition(0, 0, -3);
	lightEntity1->SetScale(5, 5, 5);

	lightEntity2 = new EngineEntity(lightMesh, dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	lightEntity2->SetPosition(0, 0, 3);
	lightEntity2->SetScale(5, 5, 5);

	lightEntity3 = new EngineEntity(lightMesh, dx::XMFLOAT3(1.0f, 0.6f, 0.2f));
	lightEntity3->SetPosition(3, 0, 2);
	lightEntity3->SetScale(5, 5, 5);

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
//	shadowMapSize = 2048;  //correct value?
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

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		entities[i]->UpdateWorldMatrix();
	}
	
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
	
	// std::cout << "x: " << camera->GetPosition().x << "\ny: " << camera->GetPosition().y << "\nz: " << camera->GetPosition().z << std::endl;

	// Access ring's position. Has index 4 in entities vector.
	if(camera->GetPosition().x > (entities[4]->GetPosition().x - 0.5f) && camera->GetPosition().x < (entities[4]->GetPosition().x + 0.5f))
	{
		if(camera->GetPosition().y > (entities[4]->GetPosition().y - 1.0f) && camera->GetPosition().y < entities[4]->GetPosition().y + 0.5f)
		{
			if (camera->GetPosition().z > entities[4]->GetPosition().z - 0.5f && camera->GetPosition().z < entities[4]->GetPosition().z + 0.5f )
			{
																							// MAX - MIN
				float randomX = -5.0f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(10.0f)));
				float randomY = 0.5f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(4.0f)));
				float randomZ = -5.0f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(10.0f)));
				while(randomZ > -1.0f && randomZ < 1.0f) // So ring doesn't end up inside monkees.
				{
					randomZ = -5.0f + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(10.0f)));
				}
				entities[4]->SetPosition(randomX, randomY, randomZ);
				camera->ResetCamera();
				currentScore++;
				std::cout << "Your current score is: " << currentScore << std::endl;
				std::cout << "The ring's position is: " << entities[4]->GetPosition().x << "  " 
					<< entities[4]->GetPosition().y << "  " << entities[4]->GetPosition().z << std::endl;

				// Win condition, reset position of ring to center and reset score.
				if(currentScore == 10)
				{
					entities[4]->SetPosition(0.0f, 3.5f, 0.0f);
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
	const float bgColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
	int i;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind render target view array and depth stencil buffer to output render pipeline
	context->OMSetRenderTargets(BUFFER_COUNT, renderTargetViewArray, depthStencilView);

	// Clear the render target buffers
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		context->ClearRenderTargetView(renderTargetViewArray[i], bgColor);
	}

	// TODO: FIXA BACKFACE CULLINGEN - VART I HELSKOTTA LÄGGER VI DET??????
	/*backfaceGeometryShader->SetFloat3("camPos", camera->GetPosition());
	backfaceGeometryShader->CopyAllBufferData();
	backfaceGeometryShader->SetShader();*/

	// Clear the depth buffer
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// TODO: FIXA BACKFACE CULLINGEN - VART I HELSKOTTA LÄGGER VI DET??????
	/*backfaceGeometryShader->SetFloat3("camPos", camera->GetPosition());
	backfaceGeometryShader->CopyAllBufferData();
	backfaceGeometryShader->SetShader();*/
	
	// Shadow stuff_______________________________
	//float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};  // Set blend factor[inconsequential, since not using]
	//context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF); // Setting the blend state
	// RenderShadowMap();

	//for (unsigned int i = 0; i < entities.size(); i++)
	//{
	//	vertexBuffer = entities[i]->GetMesh()->GetVertexBuffer();
	//	indexBuffer = entities[i]->GetMesh()->GetIndexBuffer();
	//	/*renderer.SetVertexShader(vertexShader, entities[i], camera, shadowViewMatrix, shadowProjectionMatrix);
	//	renderer.SetPixelShader(pixelShader, entities[i], camera, shadowSampler, shadowSRV);*/

	//	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//	context->DrawIndexed(entities[i]->GetMesh()->GetIndexCount(), 0, 0);
	//}
	//_____________________


	for (unsigned int i = 0; i < entities.size(); i++)
	{
		vertexBuffer = entities[i]->GetMesh()->GetVertexBuffer();
		indexBuffer = entities[i]->GetMesh()->GetIndexBuffer();

		deferredVertexShader->SetMatrix4x4("world", entities[i]->GetWorldMatrix());
		deferredVertexShader->SetMatrix4x4("view", camera->GetView());
		deferredVertexShader->SetMatrix4x4("projection", camera->GetProjection());

		//shadow
	/*	deferredVertexShader->SetMatrix4x4("shadowView", shadowViewMatrix);
		deferredVertexShader->SetMatrix4x4("shadowProj", shadowProjectionMatrix);*/

		deferredVertexShader->CopyAllBufferData();
		deferredVertexShader->SetShader();

		deferredPixelShader->SetSamplerState("Sampler", entities[i]->GetMaterial()->GetSamplerState());
		deferredPixelShader->SetShaderResourceView("Texture", entities[i]->GetMaterial()->GetTextureSRV());
		deferredPixelShader->SetShaderResourceView("NormalMap", entities[i]->GetMaterial()->GetNormalMapSRV());

		deferredPixelShader->CopyAllBufferData();
		deferredPixelShader->SetShader();


		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		
		context->DrawIndexed(entities[i]->GetMesh()->GetIndexCount(), 0, 0);
	}
	
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->ClearRenderTargetView(backBufferRTV, bgColor);
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

	dirPixelShader->SetFloat3("lightColor", dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	dirPixelShader->SetFloat3("lightDir", dx::XMFLOAT3(1.0f, -1.0f, 0.0f));

	dirPixelShader->CopyAllBufferData();
	dirPixelShader->SetShader();

	/*waterVertexShader->SetMatrix4x4("world", entities[5]->GetWorldMatrix());
	waterVertexShader->SetMatrix4x4("view", camera->GetView());
	waterVertexShader->SetMatrix4x4("projection", camera->GetProjection());

	waterVertexShader->CopyAllBufferData();
	waterVertexShader->SetShader();

	waterPixelShader->SetFloat("waterTranslation", water->GetWaterTranslation());
	waterPixelShader->SetShaderResourceView("Texture", entities[5]->GetMaterial()->GetTextureSRV());
	waterPixelShader->SetShaderResourceView("NormalMap", entities[5]->GetMaterial()->GetNormalMapSRV());
	waterPixelShader->SetSamplerState("Sampler", sampler);

	waterPixelShader->CopyAllBufferData();
	waterPixelShader->SetShader();*/


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

		lightPixelShader->SetFloat3("lightColor", lightEntities[i]->GetLightColor());
		lightPixelShader->SetFloat3("lightPosition", lightEntities[i]->GetPosition());
		lightPixelShader->CopyAllBufferData();
		lightPixelShader->SetShader();

		context->DrawIndexed(lightEntities[i]->GetMesh()->GetIndexCount(), 0, 0);
	}
	
	// Reset the render state.
	context->RSSetState(NULL);
	context->OMSetBlendState(NULL, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(NULL, 0);

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