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
	water = 0;

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
	delete water;

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
	delete shadowPixelShader;
	delete shadowVertexShader;
	delete dirPixelShader;
	delete dirVertexShader;
	delete particleVertexShader;
	delete particlePixelShader;
	delete waterVertexShader;
	delete waterPixelShader;
	delete refractionVertexShader;
	delete refractionPixelShader;
	delete skyboxVertexShader;
	delete skyboxPixelShader;

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
	SetRasterStates(width, height);
	CreateMatrices();
	LoadShaders();
	LoadMesh();
	LoadTextures();	
	CreateMaterials();
	CreateBasicGeometry();
	// SetupWater();
	LightEntityInitialize();

	// Tessellation - Displacement mapping
	tessellationAmount = 10.0f;
	HBias = 1.0f;
	HScale = 0.5f;
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::SetRasterStates(int textureWidth, int textureHeight)
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

	// SKYBOX
	/*D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rsDesc, &skyRasterState);

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dsDesc, &skyDs);*/
}

void Engine::CreateMatrices()
{
	camera = new Camera(0, 0, -10);
	camera->UpdateProjectionMatrix((float)width / height);
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

	shadowPixelShader = new SimplePixelShader(device, context);
	if (!shadowPixelShader->LoadShaderFile(L"Debug/ShadowPS.cso"))
		shadowPixelShader->LoadShaderFile(L"ShadowPS.cso");

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
	if (!waterPixelShader->LoadShaderFile(L"Debug/WaterPS.cso"))
		waterPixelShader->LoadShaderFile(L"WaterPS.cso");

	refractionVertexShader = new SimpleVertexShader(device, context);
	if (!refractionVertexShader->LoadShaderFile(L"Debug/RefractionVS.cso"))
		refractionVertexShader->LoadShaderFile(L"RefractionVS.cso");

	refractionPixelShader = new SimplePixelShader(device, context);
	if (!refractionPixelShader->LoadShaderFile(L"Debug/RefractionPS.cso"))
		refractionPixelShader->LoadShaderFile(L"RefractionPS.cso");

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
	planeMesh = new Mesh(device, "Debug/Models/cube.obj");
	lightMesh = new Mesh(device, "Debug/Models/sphere.obj");
	poolMesh = new Mesh(device, "Debug/Models/pool.obj");
}

void Engine::SetupWater()
{
	// Create water object and initialize.
	water = new Water;
	water->Initialize(device, L"Debug/Textures/NormalMaps/waterNormal.jpg", 2.5f, 1.0f);
	
	// TODO: Refraction and reflection
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

	// SKYMAP (WIP)
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/overcastMap.jpg", 0, &skySRV);

	// POOL AND WATER (WIP)
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/bathTiles.jpg", 0, &poolSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/NormalMaps/triangleTileNormals.jpg", 0, &poolNormalMapSRV);
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
	planeMaterial = new Material(vertexShader, pixelShader, planeTextureSRV, planeNormalMapSRV, sampler);
	poolMaterial = new Material(vertexShader, pixelShader, poolSRV, poolNormalMapSRV, sampler);
}

void Engine::CreateBasicGeometry()
{
	// Create a plane (ground).
	EngineEntity* ground = new EngineEntity(planeMesh, planeMaterial);

	// Create objects.
	// SPHERES
	EngineEntity* monkey = new EngineEntity(objectMesh, objectMaterial);
	EngineEntity* monkey1 = new EngineEntity(objectMesh, objectMaterial);

	// POOL FOR WATER
	EngineEntity* pool = new EngineEntity(poolMesh, poolMaterial);

	// Add plane (ground) to vector.
	entities.push_back(ground);

	// Add objects to vector.
	entities.push_back(monkey);
	entities.push_back(monkey1);

	// Add pool to vector
	entities.push_back(pool);

	monkey->SetScale(1.0f, 1.0f, 1.0f);
	monkey->SetPosition(0.0f, 1.0f, 0.0f);

	monkey1->SetScale(1.0f, 1.0f, 1.0f);
	monkey1->SetPosition(4.0f, 1.0f, 0.0f);
	monkey1->SetRotation(0.0f, 90.0f, 0.0f);

	// Bottom
	ground->SetScale(10.0f, 0.025f, 10.0f);
	ground->SetPosition(0.0f, -2.0f, 0.0f);

	pool->SetScale(1.0f, 1.0f, 1.0f);
	pool->SetPosition(0.0f, -1.9f, 0.0f);

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

void Engine::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		entities[i]->UpdateWorldMatrix();
	}
	
	for (unsigned int j = 0; j < lightEntities.size(); j++)
	{
		lightEntities[j]->UpdateWorldMatrix();
	}
}

void Engine::Draw(float deltaTime, float totalTime)
{
	const float bgcolor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	int i;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind render target view array and depth stencil buffer to output render pipeline
	context->OMSetRenderTargets(BUFFER_COUNT, renderTargetViewArray, depthStencilView);

	// Clear the render target buffers
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		context->ClearRenderTargetView(renderTargetViewArray[i], bgcolor);
	}

	// TODO: FIXA BACKFACE CULLINGEN - VART I HELSKOTTA LÄGGER VI DET??????
	/*backfaceGeometryShader->SetFloat3("camPos", camera->GetPosition());
	backfaceGeometryShader->CopyAllBufferData();
	backfaceGeometryShader->SetShader();*/

	// Clear the depth buffer
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		vertexBuffer = entities[i]->GetMesh()->GetVertexBuffer();
		indexBuffer = entities[i]->GetMesh()->GetIndexBuffer();

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

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		
		context->DrawIndexed(entities[i]->GetMesh()->GetIndexCount(), 0, 0);
	}
	
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->ClearRenderTargetView(backBufferRTV, bgcolor);
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
	dirPixelShader->SetFloat3("lightDir", dx::XMFLOAT3(1.0f, 0.0f, 0.0f));

	dirPixelShader->CopyAllBufferData();
	dirPixelShader->SetShader();

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



	//// Set up the sky shaders
	//skyboxVertexShader->SetMatrix4x4("view", camera->GetView());
	//skyboxVertexShader->SetMatrix4x4("projection", camera->GetProjection());
	//skyboxVertexShader->CopyAllBufferData();
	//skyboxVertexShader->SetShader();

	//skyboxPixelShader->SetShaderResourceView("Sky", skySRV);
	//skyboxPixelShader->CopyAllBufferData();
	//skyboxPixelShader->SetShader();

	//context->RSSetState(skyRasterState);
	//context->OMSetDepthStencilState(skyDs, 0);
	//context->DrawIndexed(planeMesh->GetIndexCount(), 0, 0);

	//context->RSSetState(NULL);

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