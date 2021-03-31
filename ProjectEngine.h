#pragma once

#define BUFFER_COUNT 3

#include <DirectXMath.h>

#include "DXInit.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "EngineEntity.h"
// #include "renderer.h"
#include "Material.h"
#include "Lights.h"

#include "WICTextureLoader.h" // Part of DXTK

class ProjectEngine : public DXInit
{
private:
	// Swapping input and/or Mesh.
	bool previousTab;
	unsigned int currentEntity;

	// Stuff to clean up later.
	std::vector<Mesh*> meshes;
	std::vector<EngineEntity*> entities;
	std::vector<EngineEntity*> lightEntities; 
	Camera* camera;

	EngineEntity* lightEntity1;
	EngineEntity* lightEntity2;
	EngineEntity* lightEntity3;
	EngineEntity* lightEntity4;
	EngineEntity* lightEntity5;
	EngineEntity* lightEntity6;
	EngineEntity* lightEntity7;
	EngineEntity* lightEntity8;

	Mesh* capsuleMesh; 
	Mesh* planeMesh;

	//Renderer renderer

	Material* capsuleMaterial;
	Material* planeMaterial;

	void SetDeferredSetup(int width, int height); // Parameters are for texture
	void CreateMatrices();
	void LoadShaders();
	void LoadMesh();
	void LoadTextures();
	void CreateMaterials();
	void CreateGeometry();
	void LightEntityInit();

	// Texture related schtuff
	ID3D11ShaderResourceView* capsuleTextureSRV;
	ID3D11ShaderResourceView* capsuleNormalMapSRV;
	ID3D11ShaderResourceView* planeTextureSRV;
	ID3D11ShaderResourceView* planeNormalMapSRV;
	ID3D11SamplerState* sampler;

	// Init buffers.
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Rendertargets, deferred rendering.
	ID3D11RenderTargetView* renderTargetViewArray[(int)BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[(int)BUFFER_COUNT];
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthState;
	ID3D11RasterizerState* rasterizer;
	ID3D11BlendState* blendState;

	// Wrappers for DX shaders.
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* deferredVertexShader;
	SimplePixelShader* deferredPixelShader;
	SimpleVertexShader* backBufferVertexShader;
	SimplePixelShader* backBufferPixelShader;
	SimpleVertexShader* lightVertexShader;
	SimplePixelShader* lightPixelShader;
	SimpleVertexShader* dirVertexShader;
	SimplePixelShader* dirPixelShader;

	// Matrices
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix; 


	// To keep track of previous mouseposition, this helps determine how far the mouse moves in a single frame.
	POINT prevMousePos;

public:
	ProjectEngine(HINSTANCE hInstance);
	~ProjectEngine();


	void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	void MouseDown(WPARAM buttonState, int x, int y);
	void MouseUp(WPARAM buttonState, int x, int y);
	void MouseMove(WPARAM buttonState, int x, int y);
	void MouseWheel(float wheekDelta, int x, int y);
};

