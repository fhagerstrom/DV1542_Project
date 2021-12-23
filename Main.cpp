#include "Defines.h"
#include "Timer.h"
#include "Camera.h"
#include "WindowHelper.h"
#include "DirectXHelper.h"
#include "Pipeline.h"
#include "Shaders.h"
#include "RenderParticles.h"
#include "Lighting.h"
#include "RenderModels.h"
#include "RenderShadows.h"



// Function to clear the window
void clearView(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, const GeometryBuffer& gBuffer)
{
	// Background Color
	float backGroundColor[4] = { 0.5f, 0.5f, 0.5f, 0 };

	// Clean SRVs to clean the OS
	ID3D11ShaderResourceView* nullSrvs[gBuffer.NROFBUFFERS] = { nullptr };
	immediateContext->PSSetShaderResources(0, gBuffer.NROFBUFFERS, nullSrvs);

	// Clear the DSV
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);

	// Reset the RTVs
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		// Clear the RTVs with the BG-color
		immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[i], backGroundColor);

	}
	immediateContext->OMSetRenderTargets(gBuffer.NROFBUFFERS, gBuffer.gBuffergBufferRtv, dsView);
}

// Update function to update the camera, the directional light
void update(ID3D11DeviceContext* immediateContext, float deltaTime, Camera& camera,
	Particles* particlesystem, Lighting& light, ID3D11Buffer* lightBuffer,
	const std::vector<Model*> models, Model* monkey)
{
	camera.Update(deltaTime);
	particlesystem->Update(immediateContext, deltaTime);

	// Rotation of Monkey-model
	XMFLOAT4 tempRotation;
	XMStoreFloat4(&tempRotation, monkey->GetRotation());

	tempRotation.y += 1.3f * deltaTime;

	XMVECTOR rotation = XMVectorSet(tempRotation.x, tempRotation.y, tempRotation.z, tempRotation.z);
	monkey->SetRotation(rotation);
	monkey->Update();


}

// Geometry Pass for deferred rendering (and shadows)
void RenderGBufferPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView,
	D3D11_VIEWPORT& viewport, ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred,
	ID3D11SamplerState* sampler, GeometryBuffer gBuffer, Particles* particles,
	RenderParticles* particleRenderer, RenderModels* modelRenderer, const std::vector <Model*>& models,
	RenderShadows* shadowRenderer, ID3D11RasterizerState*& rasterizerStateWireFrame, ID3D11RasterizerState*& rasterizerStateSolid,
	ID3D11Device* device, ID3D11SamplerState* clampSampler)
{
	// Binds the shadowmap (sets resources)
	Shaders::shadowmap->Bind(immediateContext);

	// Set the HS, DS and GS shaders to NULL, used in the different renderers
	immediateContext->HSSetShader(NULL, NULL, 0);
	immediateContext->DSSetShader(NULL, NULL, 0);
	immediateContext->GSSetShader(NULL, NULL, 0);

	// Loops through the models-vector and render shadows
	for (auto& model : models)
		shadowRenderer->Render(immediateContext, model);
	


	// Reset the Viewport for the next pass (geometry pass)
	immediateContext->RSSetViewports(NULL, NULL);

	// Set all the Samplers, Rasterizerstate, viewport and geometryshader (GS used for culling)
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->PSSetSamplers(1, 1, &clampSampler);
	immediateContext->DSSetSamplers(0, 1, &sampler);
	immediateContext->DSSetSamplers(1, 1, &clampSampler);
	immediateContext->RSSetState(rasterizerStateSolid);
	immediateContext->GSSetShader(Shaders::gShader, nullptr, 0); //geometry shader

	clearView(immediateContext, rtv, dsView, gBuffer); 	// Clear the window for next render pass

	// Render the models using different renderers (modelRenderer,  particleRenderer)
	for (auto& model : models)
	{
		modelRenderer->Render(device, immediateContext, model, false);
	}	

	//render particles
	particleRenderer->Render(immediateContext, particles);
}

// Light pass -- renders the geometry and lighting on the final screen quad
void RenderLightPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred, ID3D11SamplerState* sampler,
	GeometryBuffer gBuffer, ID3D11PixelShader* lightPShaderDeferred, ID3D11VertexShader* lightVShaderDeferred,
	ID3D11InputLayout* renderTargetMeshInputLayout, ID3D11Buffer* screenQuadMesh, Lighting& light,
	ID3D11Buffer* dirLightBuffer, Camera camera, ID3D11Buffer* cameraPos, ID3D11SamplerState* clampSampler
)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	immediateContext->OMSetRenderTargets(1, &rtv, dsView);										// Backbuffer
	immediateContext->IASetVertexBuffers(0, 1, &screenQuadMesh, &stride, &offset);				// quads vertexbuffer set
	immediateContext->IASetInputLayout(renderTargetMeshInputLayout);							// set the Input Layout
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);			// triangletstrip since we know how many points we have
	immediateContext->VSSetShader(lightVShaderDeferred, nullptr, 0);							// setting the shaders
	immediateContext->PSSetShader(lightPShaderDeferred, nullptr, 0);							// setting the shaders

	immediateContext->PSSetShaderResources(0, gBuffer.NROFBUFFERS, gBuffer.gBufferSrv);
	immediateContext->PSSetShaderResources(8, 1, Shaders::shadowmap->GetSRV());				// Get the shadow-map SRV and send it to the PixelShader
	immediateContext->PSSetConstantBuffers(0, 1, &dirLightBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &cameraPos);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->RSSetState(nullptr);

	immediateContext->Draw(4, 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// Width and height of the window
	const UINT WIDTH = 1600;
	const UINT HEIGHT = 900;
	HWND window;

	// Console window-function
	//RedirectIOToConsole();

	// Camera
	Camera camera(XM_PIDIV4, (float)WIDTH / (float)HEIGHT, 0.1, 100, 2.5, 5.0f, { 15.0f, 10.0f ,0.0f });
	ID3D11Buffer* cameraBuffer;

	// Time
	Timer timer;
	float deltatime = 0.0f;

	ID3D11Device* device;								// Used to create resources.
	ID3D11DeviceContext* immediateContext;				// Generates rendering commands.
	IDXGISwapChain* swapChain;							// This function changes the back buffer (rtv) and the display screen.
	ID3D11RenderTargetView* rtv;						// This variable is a pointer to an object that contains all the information about the rendering object. 
	ID3D11DepthStencilView* dsView;						// The Depth / Stencil Buffer stores depth information for the pixels to be rendered.
	D3D11_VIEWPORT viewport;							// Defines the dimensions of a viewport.
	ID3D11Texture2D* dsTexture;							// An ID3D11Texture2D is an object that stores a flat image.
	ID3D11RasterizerState* rasterizerStateWireFrame;	// RasterizerState - wireframe
	ID3D11RasterizerState* rasterizerStateSolid;		// RasterizerState - solid

	ID3D11SamplerState* wrapSampler;					// Wrap sampler - wraps or "loops" the texture
	ID3D11SamplerState* clampSampler;					// Clamp sampler - clamps the UV values [0.0 - 1.0]

	//Deferred
	ID3D11PixelShader* lightPShaderDeferred;			// Pixel Shader for the light pass ( quad )
	ID3D11VertexShader* lightVShaderDeferred;		    // Vertex Shader for the light pass ( quad ) 
	ID3D11PixelShader* pShaderDeferred;					// Pixel Shader for the geometry pass
	ID3D11VertexShader* vShaderDeferred;				// Vertex Shader for the geometry pass
	ID3D11InputLayout* renderTargetMeshInputLayout;		// Input layout for the quad covering the sqreen							
	ID3D11Buffer* screenQuadMesh;						// Quad covering the screen used in the deferred rendering- light pass

	
	//Gbuffer struct
	GeometryBuffer gBuffer;
	gBuffer.screenWidth = WIDTH;
	gBuffer.screenHeight = HEIGHT;
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		gBuffer.gBufferTexture[i] = nullptr;
	}

	// Directional light (both lighting and shadows)
	Lighting light(14);
	ID3D11Buffer* lightBuffer;

	// Creates the window
	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}
	// Setups DirectX11
	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext,
		swapChain, rtv, dsTexture, dsView, viewport, gBuffer))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -2;
	}

	if (!SetupPipeline(device, wrapSampler, pShaderDeferred, vShaderDeferred, lightPShaderDeferred, lightVShaderDeferred,
		renderTargetMeshInputLayout, screenQuadMesh, rasterizerStateWireFrame, rasterizerStateSolid, clampSampler))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -3;
	}

	// Creating a vector that stores the models
	std::vector <Model*>models;

	// Creating a new model for each mesh in the scene
	
	Model* monkey = new Model(device, "Statue", { 3.0f, 5.0f, 0.0f }, { 0.0f, 0.0f, XM_PIDIV4 }, { 0.6f, 0.6f, 0.6f });
	models.push_back(monkey);
	/*
	Model* plane = new Model(device, "plane", { 3.0f, 5.0f, 0.0f }, { 0.0f, 0.0f, XM_PIDIV4 }, { 0.6f, 0.6f, 0.6f });
	models.push_back(monkey);
	*/
	Model* cameraModel = new Model(device, "cube", { 0.0f, -5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.05f, 0.05f, 0.05f });
	models.push_back(cameraModel);
	/*
	Model* dust = new Model(device, "buildings", { -10.0f, -0.2f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 1.1f, 1.1f, 1.1f });
	models.push_back(dust);

	Model* statue = new Model(device, "Statue", { 8.0f, 1.5f, 7.0f }, { 0.0f, XM_PIDIV2 + 0.623598776f, 0.0f }, { 0.25f, 0.25f, 0.25f });
	models.push_back(statue);

	Model* water = new Model(device, "water", { 0.0f, 5.0f, 8.0f }, { 0.0f, XM_PIDIV2 - 0.623598776f , -XM_PIDIV2 }, { 10.0f, 10.0f, 10.0f });

	Model* sword = new Model(device, "sword", { 8.0f, 3.0f, -7.0f }, { -XM_PIDIV4, XM_PIDIV2, 0 }, { 0.7f, 0.7, 0.7f });
	models.push_back(sword);

	Model* ball = new Model(device, "ball", { 9.0f, 0.0f, 8.5f }, { 0.0f, 0.0f, 0 }, { 0.5f, 0.5f, 0.5f });
	models.push_back(ball);

	Model* board = new Model(device, "board", { 8.5f, 0.0f, 9.0f }, { 0.0f, XM_PIDIV4, 0 }, { 0.5f, 0.5f, 0.5f });
	models.push_back(board);
	*/

	Particles* particles = new Particles(device, 200, 5, 1, { 60, 25, 60 }, { 0, 20, 0 });
	RenderParticles* particlesRenderer = new RenderParticles(device);
	RenderModels* modelRenderer = new RenderModels(device);
	RenderShadows* shadowRenderer = new RenderShadows(device);

	Shaders::Initialize(device, modelRenderer->GetByteCode());

	CreateBuffer(device, lightBuffer, sizeof(Lighting::Data));
	UpdateBuffer(immediateContext, lightBuffer, light.data);
	CreateBuffer(device, cameraBuffer, sizeof(XMFLOAT4));

	MSG msg = {};

	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		timer.Start();

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
			
		update(immediateContext, deltatime, camera, particles, light, lightBuffer, models, monkey);

		Shaders::Update(immediateContext, camera, light);

		RenderGBufferPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred,
			wrapSampler, gBuffer, particles, particlesRenderer, modelRenderer, models, shadowRenderer,
			rasterizerStateWireFrame, rasterizerStateSolid, device, clampSampler);

		RenderLightPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred,
			wrapSampler, gBuffer, lightPShaderDeferred, lightVShaderDeferred, renderTargetMeshInputLayout, screenQuadMesh,
			light, lightBuffer, camera, cameraBuffer, clampSampler);

		swapChain->Present(0, 0); // Presents a rendered image to the user.

		deltatime = timer.DeltaTime();
	}

	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		gBuffer.gBuffergBufferRtv[i]->Release();
		gBuffer.gBufferSrv[i]->Release();
		gBuffer.gBufferTexture[i]->Release();
	}

	// Clear the vector
	models.clear();

	// Vet inte var alla minnesläckor finns, kolla outputten efter live objects ( ny grej som sophia löste (otippat))
	// Tror det är i material & texture men klurigt hur det ska gå till, tror att vi försöker fixa så mycket som möjligt sen skriva kommentarer och låta sophia lösa det eller något

	monkey->Shutdown();
	cameraModel->Shutdown();
	/*
	dust->Shutdown();
	sword->Shutdown();
	statue->Shutdown();
	
	terrain->Shutdown();
	water->Shutdown();
	ball->Shutdown();
	board->Shutdown();
	*/
	delete monkey;
	delete cameraModel;
	/*
	delete dust;
	delete sword;
	delete statue;
	delete water;

	delete terrain;
	delete board;
	delete ball;
	*/
	particlesRenderer->ShutDown();
	shadowRenderer->ShutDown();
	modelRenderer->ShutDown();

	Shaders::Shutdown();
	particles->Shutdown();
	delete particles;
	delete particlesRenderer;
	delete modelRenderer;
	delete shadowRenderer;
	rasterizerStateWireFrame->Release();
	rasterizerStateSolid->Release();
	cameraBuffer->Release();
	lightBuffer->Release();
	vShaderDeferred->Release();
	pShaderDeferred->Release();
	screenQuadMesh->Release();
	renderTargetMeshInputLayout->Release();
	lightVShaderDeferred->Release();
	lightPShaderDeferred->Release();
	clampSampler->Release();
	wrapSampler->Release();
	dsTexture->Release();
	dsView->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}
