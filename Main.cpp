#include "Defines.h"
#include "Timer.h"
#include "Camera.h"
#include "WindowHelper.h"
#include "DxHandler.h"
#include "Pipeline.h"
#include "Shaders.h"
#include "RenderParticles.h"
#include "Lighting.h"
#include "RenderModels.h"
#include "RenderShadows.h"

#include "QuadTree.h"
#include "Frustum.h"

//console shite
#include<io.h>
#include<fcntl.h>

// Console Function, got this thingy from canvas
void RedirectIOToConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");

	freopen_s(&fp, "CONOUT$", "w", stdout);
}


// Function to clear the window
void clearView(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, const GBuffer& gBuffer)
{
	// Background Color
	float backGroundColor[4] = { 0.5f, 0.5f, 0.5f, 0 };

	// Clean SRVs
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
	const std::vector<Model*> models, Model* monkey, Model* monkey2, Model* monkey3)
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

	XMStoreFloat4(&tempRotation, monkey2->GetRotation());
	tempRotation.x += 1.3f * deltaTime;
	rotation = XMVectorSet(tempRotation.x, tempRotation.y, tempRotation.z, tempRotation.z);
	monkey2->SetRotation(rotation);
	monkey2->Update();

	XMStoreFloat4(&tempRotation, monkey3->GetRotation());
	tempRotation.z += 1.3f * deltaTime;
	rotation = XMVectorSet(tempRotation.x, tempRotation.y, tempRotation.z, tempRotation.z);
	monkey3->SetRotation(rotation);
	monkey3->Update();

	
}

// Geometry Pass for deferred rendering (and shadows)
void RenderGBufferPass(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView,
	D3D11_VIEWPORT& viewport, ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred,
	ID3D11SamplerState* sampler, GBuffer gBuffer, Particles* particles,
	RenderParticles* particleRenderer, RenderModels* modelRenderer, const std::vector <Model*>& models,
	RenderShadows* shadowRenderer, ID3D11RasterizerState*& rasterizerStateWireFrame, ID3D11RasterizerState*& rasterizerStateSolid,
	ID3D11Device* device, ID3D11SamplerState* clampSampler, Frustum* frustum, QuadTree* quadTree, Camera camera)
{
	// Binds the shadowmap (sets resources)
	Shaders::shadowmap->Bind(context);

	modelRenderer->modelsRendered = 0;
	
	// Set the HS, DS and GS shaders to NULL, used in the different renderers
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);

	// Loops through the models-vector and render shadows
	for (auto& model : models)
	{
		shadowRenderer->Render(context, model);
	}


	// Reset the Viewport for the next pass (geometry pass)
	context->RSSetViewports(NULL, NULL);

	// Set all the Samplers, Rasterizerstate, viewport and geometryshader (GS used for culling)
	context->RSSetViewports(1, &viewport);
	context->PSSetSamplers(0, 1, &sampler);
	context->PSSetSamplers(1, 1, &clampSampler);
	context->DSSetSamplers(0, 1, &sampler);
	context->DSSetSamplers(1, 1, &clampSampler);
	context->RSSetState(rasterizerStateSolid);
	context->GSSetShader(Shaders::gShader, nullptr, 0); //geometry shader

	clearView(context, rtv, dsView, gBuffer); 	// Clear the window for next render pass

	// Time for frustum stuff_________________________________________________________________________________________
	frustum->ConstructFrustum(camera.GetFarZ(), camera.GetPerspectiveMatrix(), camera.GetViewMatrix()); //make the actual frustum
	
	quadTree->Render(frustum, device, context, modelRenderer);
	


	//________________________________________________________________________________________________________________


	// Render the models using different renderers (modelRenderer,  particleRenderer)
	for (auto& model : models) //används detta för icke frustum, fixa en annan loop för frustum (
	{
		if( model->mesh.name == "monkey" || model->mesh.name == "plane")
			modelRenderer->Render(device, context, model, false);
	}	
	std::cout << "Models rendered: " << modelRenderer->modelsRendered << std::endl;
	//render particles
	particleRenderer->Render(context, particles);
	

}

// Light pass -- renders the geometry and lighting on the final screen quad
void RenderLightPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred, ID3D11SamplerState* sampler,
	GBuffer gBuffer, ID3D11PixelShader* lightPShaderDeferred, ID3D11VertexShader* lightVShaderDeferred,
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
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 768;
	HWND window;

	//Console window-function
	RedirectIOToConsole();

	// Camera
	Camera camera(XM_PIDIV4, (float)WINWIDTH / (float)WINHEIGHT, 0.1, 100, 2.5, 5.0f, { 15.0f, 10.0f ,0.0f });
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
	GBuffer gBuffer;
	gBuffer.screenWidth = WINWIDTH;
	gBuffer.screenHeight = WINHEIGHT;
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
	if (!SetupD3D11(WINWIDTH, WINHEIGHT, window, device, immediateContext,
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
																									// "depth" "height" "width"
	Model* monkey = new Model(device, "monkey", { 3.0f, 2.5f, 2.0f }, { 0.0f, XM_PIDIV4, 0.0f }, { 0.6f, 0.6f, 0.6f });
	models.push_back(monkey);

	Model* monkey2 = new Model(device, "monkey", { 1.0f, 2.5f, 0.0f }, { XM_PIDIV4, 0.0f, 0.0f }, { 0.6f, 0.6f, 0.6f });
	models.push_back(monkey2);

	Model* monkey3 = new Model(device, "monkey", { 5.0f, 2.5f, 0.0f }, { 0.0f, 0.0f, XM_PIDIV4 }, { 0.6f, 0.6f, 0.6f });
	models.push_back(monkey3);
	
	Model* ground = new Model(device, "plane", { 3.0f, 1.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 0.001f, 3.0f });
	models.push_back(ground);
	
	Model* sphere = new Model(device, "sphere", { 8.0f, 2.5f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere);
	
	Model* sphere2 = new Model(device, "sphere", { -8.0f, 2.5f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere2);

	Model* sphere3 = new Model(device, "sphere", { -8.0f, 2.5f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere3);

	Model* sphere4 = new Model(device, "sphere", { 8.0f, 2.5f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere4);
	
	Model* sphere5 = new Model(device, "sphere", { 6.0f, 2.5f, -2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere5);

	Model* sphere6 = new Model(device, "sphere", { 6.0f, 2.5f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere6);

	Model* sphere7 = new Model(device, "sphere", { -6.0f, 2.5f, -2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere7);

	Model* sphere8 = new Model(device, "sphere", { -6.0f, 2.5f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere8);

	Model* sphere9 = new Model(device, "sphere", { 15.0f, 2.5f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere9);

	Model* sphere10 = new Model(device, "sphere", { 15.0f, 2.5f, -2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere10);

	Model* sphere11 = new Model(device, "sphere", { 12.0f, 2.5f, -4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere11);

	Model* sphere12 = new Model(device, "sphere", { 20, 2.5f, -7.0f }, { 0.0f, 0.0f, 0.0f }, { 0.02f, 0.02f, 0.02f });
	models.push_back(sphere12);

	

	Particles* particles = new Particles(device, 100, 50, 20, { 60, 25, 60 }, { 0, 20, 0 });
	RenderParticles* particlesRenderer = new RenderParticles(device);
	RenderModels* modelRenderer = new RenderModels(device);
	RenderShadows* shadowRenderer = new RenderShadows(device);

	//for culling
	Frustum* frustum = new Frustum;
	QuadTree* quadTree = new QuadTree;
	bool initSuccessful = quadTree->Initialize(models);
	if (!initSuccessful)
	{
		std::cout << "ERROR! COULD NOT INITIALIZE QUAD TREE IN MAIN!" << std::endl;
		return -4;
	}
	//_______________________________________________________________________________________________
	

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
		
		update(immediateContext, deltatime, camera, particles, light, lightBuffer, models, monkey, monkey2, monkey3);

		Shaders::Update(immediateContext, camera, light);

		RenderGBufferPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred,
			wrapSampler, gBuffer, particles, particlesRenderer, modelRenderer, models, shadowRenderer,
			rasterizerStateWireFrame, rasterizerStateSolid, device, clampSampler, frustum, quadTree, camera);

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

	monkey->Shutdown();
	monkey2->Shutdown();
	monkey3->Shutdown();
	ground->Shutdown();
	sphere->Shutdown();
	sphere2->Shutdown();
	sphere3->Shutdown();
	sphere4->Shutdown();
	sphere5->Shutdown();
	sphere6->Shutdown();
	sphere7->Shutdown();
	sphere8->Shutdown();
	sphere9->Shutdown();
	sphere10->Shutdown();	
	sphere11->Shutdown();
	sphere12->Shutdown();



	if (quadTree)
	{
		quadTree->Shutdown();
		delete quadTree;
		quadTree = 0;
	}

	if (frustum)
	{
		delete frustum;
		frustum = 0;
	}
	
	delete monkey;
	delete monkey2;
	delete monkey3;
	delete ground;
	delete sphere;
	delete sphere2;
	delete sphere3;
	delete sphere4;
	delete sphere5;
	delete sphere6;
	delete sphere7;
	delete sphere8;





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
