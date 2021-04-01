#include "ProjectEngine.h"

namespace dx = DirectX;

ProjectEngine::ProjectEngine(HINSTANCE hInstance) : DXInit(hInstance, "3D Project", 1024, 768, true)
{
	vertexBuffer = 0;
	indexBuffer = 0;
	vertexShader = 0;
	pixelShader = 0;
	camera = 0;

	for(int i = 0; i < 3; i++)
	{
		renderTargetViewArray[i] = 0;
		shaderResourceViewArray[i] = 0;
	}

	depthStencilBuffer = 0;
	depthStencilView = 0;

	#if defined(DEBUG) || defined(_DEBUG)
		// Setup console window for debug purposes.
		CreateCmdWindow(500, 120, 32, 120);
		printf("Console window created successfully!");
	#endif
	}

ProjectEngine::~ProjectEngine()
{
	// Resources to delete and release.
	delete camera;
	delete pixelShader;
	delete vertexShader;

	for(auto& mesh : meshes)
	{
		delete mesh;
	}

	delete planeMaterial;

	for(auto& entity : entities)
	{
		delete entity;
	}
	for(auto& lightEntity : lightEntities)
	{
		delete lightEntity;
	}

	sampler->Release();
	capsuleTextureSRV->Release();
	capsuleNormalMapSRV->Release();
	planeTextureSRV->Release();
	planeNormalMapSRV->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	rasterizer->Release();
}

void ProjectEngine::Init()
{
	SetDeferredSetup(width, height);
	CreateMatrices();
	LoadShaders();
	LoadMesh();
	LoadTextures();
	CreateMaterials();
	CreateGeometry();
	LightEntityInit();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ProjectEngine::SetDeferredSetup(int width, int height)
{
	unsigned int i;

	// Texture desc.
	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* renderTargetTextureArray[(int)BUFFER_COUNT];

	for (i = 0; i < BUFFER_COUNT; ++i)
	{
		device->CreateTexture2D(&textureDesc, nullptr, &renderTargetTextureArray[i]);
	}

	// Render target view desc.
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc32 = {};
	rtvDesc32.Format = textureDesc.Format;
	rtvDesc32.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // Resource accessed as a 2D texture.
	rtvDesc32.Texture2D.MipSlice = 0;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc8 = {};
	rtvDesc8.Format = textureDesc.Format;
	rtvDesc8.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // Resource accessed as a 2D texture.
	rtvDesc8.Texture2D.MipSlice = 0;

	// Create RTV
	device->CreateRenderTargetView(renderTargetTextureArray[0], &rtvDesc32, &renderTargetViewArray[0]);
	device->CreateRenderTargetView(renderTargetTextureArray[1], &rtvDesc32, &renderTargetViewArray[1]);
	device->CreateRenderTargetView(renderTargetTextureArray[2], &rtvDesc8, &renderTargetViewArray[2]);

	// Shader resource desc.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		device->CreateShaderResourceView(renderTargetTextureArray[i], &srvDesc, &shaderResourceViewArray[i]);
	}

	// Depth buffer desc.
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // Depth buffer only go with black and white.
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);

	// Depth Stencil desc.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);

	// Rasterizer desc.
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = false;

	device->CreateRasterizerState(&rasterizerDesc, &rasterizer);

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

	// Depth stencil desc.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthStencilDesc, &depthState);
}

void ProjectEngine::CreateMatrices()
{
	camera = new Camera(0, 0, -5);
	camera->UpdateProjMatrix((float)width / height);
}

void ProjectEngine::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFiles(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFiles(L"Debug/VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFiles(L"Debug/PixelShader.cso"))
		pixelShader->LoadShaderFiles(L"Debug/PixelShader.cso");

	deferredVertexShader = new SimpleVertexShader(device, context);
	if (!deferredVertexShader->LoadShaderFiles(L"Debug/DeferredVertexShader.cso"))
		deferredVertexShader->LoadShaderFiles(L"Debug/DeferredVertexShader.cso");

	deferredPixelShader = new SimplePixelShader(device, context);
	if (!deferredPixelShader->LoadShaderFiles(L"Debug/DeferredPixelShader.cso"))
		deferredPixelShader->LoadShaderFiles(L"Debug/DeferredPixelShader.cso");

	backBufferVertexShader = new SimpleVertexShader(device, context);
	if (!backBufferVertexShader->LoadShaderFiles(L"Debug/BackBufferVertexShader.cso"))
		backBufferVertexShader->LoadShaderFiles(L"Debug/BackBufferVertexShader.cso");

	backBufferPixelShader = new SimplePixelShader(device, context);
	if (!backBufferPixelShader->LoadShaderFiles(L"Debug/BackBufferPixelShader.cso"))
		backBufferPixelShader->LoadShaderFiles(L"Debug/BackBufferPixelShader.cso");

	lightVertexShader = new SimpleVertexShader(device, context);
	if (!lightVertexShader->LoadShaderFiles(L"Debug/LightVertexShader.cso"))
		lightVertexShader->LoadShaderFiles(L"Debug/LightVertexShader.cso");

	lightPixelShader = new SimplePixelShader(device, context);
	if (!lightPixelShader->LoadShaderFiles(L"Debug/LightPixelShader.cso"))
		lightPixelShader->LoadShaderFiles(L"Debug/LightPixelShader.cso");

	dirVertexShader = new SimpleVertexShader(device, context);
	if (!dirVertexShader->LoadShaderFiles(L"Debug/Debug/DirVertexShader.cso"))
		dirVertexShader->LoadShaderFiles(L"Debug/DirVertexShader.cso");

	dirPixelShader = new SimplePixelShader(device, context);
	if (!dirPixelShader->LoadShaderFiles(L"Debug/Debug/DirPixelShader.cso"))
		dirPixelShader->LoadShaderFiles(L"Debug/DirPixelShader.cso");
}

void ProjectEngine::LoadMesh()
{
	capsuleMesh = new Mesh(device, "Debug/Models/Capsule.obj");
	planeMesh = new Mesh(device, "Debug/Models/Cube.obj");
}

void ProjectEngine::LoadTextures()
{
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/mixedrust.jpg", 0, &capsuleTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/plastic.jpg", 0, &capsuleNormalMapSRV);

	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/grass.jpg", 0, &planeTextureSRV);
	dx::CreateWICTextureFromFile(device, context, L"Debug/Textures/woodplanks.jpg", 0, &planeNormalMapSRV);
}

void ProjectEngine::CreateMaterials()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Use linear interpolation for minification, magnification, and mip-level sampling.
	samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);

	capsuleMaterial = new Material(vertexShader, pixelShader, capsuleTextureSRV, capsuleNormalMapSRV, sampler);
	planeMaterial = new Material(vertexShader, pixelShader, planeTextureSRV, planeNormalMapSRV, sampler);
}

void ProjectEngine::CreateGeometry()
{
	// Spawn "items".
	EngineEntity* capsule = new EngineEntity(capsuleMesh, capsuleMaterial);
	EngineEntity* capsule1 = new EngineEntity(capsuleMesh, capsuleMaterial);
	EngineEntity* capsule2 = new EngineEntity(capsuleMesh, capsuleMaterial);

	// Create "planes".
	EngineEntity* plane = new EngineEntity(planeMesh, planeMaterial);
	// EngineEntity* plane1 = new EngineEntity(planeMesh, planeMaterial);
	// EngineEntity* plane2 = new EngineEntity(planeMesh, planeMaterial);

	entities.push_back(capsule);
	entities.push_back(capsule1);
	entities.push_back(capsule2);

	entities.push_back(plane);
	// entities.push_back(plane1);
	// entities.push_back(plane2);

	// Set scales and positions of items.
	capsule->SetPosition(0.0f, 0.0f, 0.0f);
	capsule->SetScale(1.0f, 1.0f, 1.0f);

	capsule1->SetPosition(2.0f, 0.0f, 0.0f);
	capsule1->SetScale(1.0f, 1.0f, 1.0f);

	capsule2->SetPosition(-2.0f, 0.0f, 0.0f);
	capsule2->SetScale(1.0f, 1.0f, 1.0f);

	// Bottom
	plane->SetPosition(0.0f, -3.0f, 0.0f);
	plane->SetScale(5.0f, 0.05f, 7.0f);

	// *Insert more items here*

	currentEntity = 0;
}

void ProjectEngine::LightEntityInit()
{
	// @TODO: Create lightEntity objects and set positions and size.
}

void ProjectEngine::Update(float deltaTime, float totalTime)
{
	camera->Update(deltaTime);

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		entities[i]->UpdateWorldMatrix();
	}

	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// **UPDATE WORLD MATRIX FOR LIGHTENTITIES HERE**
}

void ProjectEngine::Draw(float deltaTime, float totalTime)
{
	unsigned int i;
	const float bgColour[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind RTV and depth stencil buffer to output merger.
	context->OMSetRenderTargets(BUFFER_COUNT, renderTargetViewArray, depthStencilView);

	// Clear RTV buffers.
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		context->ClearRenderTargetView(renderTargetViewArray[i], bgColour);
	}

	// Clear depth buffer.
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (i = 0; i < entities.size(); i++)
	{
		ID3D11Buffer* vBuffer = entities[i]->GetMesh()->GetVertexBuffer();
		ID3D11Buffer* iBuffer = entities[i]->GetMesh()->GetIndexBuffer();

		deferredVertexShader->SetMatrix4x4("world", entities[i]->GetWorldMatrix());
		deferredVertexShader->SetMatrix4x4("view", camera->GetView());
		deferredVertexShader->SetMatrix4x4("projection", camera->GetProjection());
		deferredVertexShader->CopyAllBufferData();
		deferredVertexShader->SetShader();

		deferredPixelShader->SetSamplerState("sampler", entities[i]->GetMaterial()->GetSamplerState());
		deferredPixelShader->SetShaderResourceView("texture", entities[i]->GetMaterial()->GetTextureSRV());
		deferredPixelShader->SetShaderResourceView("normalMap", entities[i]->GetMaterial()->GetNormalMapSRV());
		deferredPixelShader->CopyAllBufferData();
		deferredPixelShader->SetShader();

		context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
		context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(entities[i]->GetMesh()->GetIndexCount(), 0, 0);
	}

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->ClearRenderTargetView(backBufferRTV, bgColour);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	context->RSSetState(rasterizer);
	float blend[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthState, 0);

	dirVertexShader->SetShader();

	dirPixelShader->SetShaderResourceView("Position", shaderResourceViewArray[0]);
	dirPixelShader->SetShaderResourceView("NormalMap", shaderResourceViewArray[1]);
	dirPixelShader->SetShaderResourceView("Texture", shaderResourceViewArray[2]);
	backBufferPixelShader->SetSamplerState("Sampler", sampler);

	dirPixelShader->SetFloat3("lightColor", dx::XMFLOAT3(0.5f, 0.5f, 0.5f));
	dirPixelShader->SetFloat3("lightDirection", dx::XMFLOAT3(10.0f, -10.0f, 0.0f));

	dirPixelShader->CopyAllBufferData();
	dirPixelShader->SetShader();

	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);

	context->Draw(3, 0);
	context->RSSetState(nullptr);
	context->OMSetBlendState(nullptr, blend, 0xFFFFFFFF);
	context->OMSetDepthStencilState(nullptr, 0);

	swapChain->Present(0, 0);
}


