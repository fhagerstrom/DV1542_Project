#pragma once
#define BUFFER_COUNT 3
// #define MAX_LIGHTS 2

#include <DirectXMath.h>
#include <iostream>

#include "DXInit.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "EngineEntity.h"
#include "Renderer.h"
#include "Material.h"
#include "Lights.h"
#include "Water.h"
#include "Vertex.h"
#include "WICTextureLoader.h" // DirectXTK

class Engine : public DXInit
{
private:
	// Input and mesh swapping.
	bool prevTab;
	unsigned int currentEntity;

	// Keep track of objects and entities to clean up.
	std::vector<Mesh*> meshes;
	std::vector<EngineEntity*> entities;
	std::vector<EngineEntity*> lightEntities;
	Camera* camera;

	// Add lights.
	EngineEntity* lightEntity1;
	EngineEntity* lightEntity2;
	EngineEntity* lightEntity3;
	EngineEntity* lightEntity4;
	EngineEntity* lightEntity5;
	EngineEntity* lightEntity6;
	EngineEntity* lightEntity7;
	EngineEntity* lightEntity8;
	EngineEntity* lightEntity9;

	// Mesh objects.
	Mesh* objectMesh;
	Mesh* planeMesh;
	Mesh* lightMesh;
	Mesh* poolMesh;
	Mesh* waterMesh;

	// Material class objects.
	Material* objectMaterial;
	Material* planeMaterial;
	Material* poolMaterial;
	Material* waterMaterial;

	// Water related objects.
	Water* water;

	// Sky related schtuff.
	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* rsSky;
	ID3D11DepthStencilState* dsSky;

	// Displacement mapping.
	float tessellationAmount;
	float HScale;
	float HBias;

	// Lights.
	PointLight pointLight1;
	PointLight pointLight2;
	PointLight pointLight3;
	PointLight pointLight4;

	SpotLight spotLight1;
	SpotLight spotLight2;

	// Initialization helper functions.
	void SetDefferedSetup(int textureWidth, int textureHeight);
	void CreateMatrices();
	void LoadShaders();
	void LoadMesh();
	void LoadTextures();
	void CreateMaterials();
	void CreateBasicGeometry();
	void SetupWater();
	void LightEntityInitialize();

	// Texture related schtuff.
	ID3D11ShaderResourceView* objectTextureSRV;
	ID3D11ShaderResourceView* objectNormalMapSRV;
	ID3D11ShaderResourceView* planeTextureSRV;
	ID3D11ShaderResourceView* planeNormalMapSRV;
	ID3D11ShaderResourceView* waterNormalMapSRV;
	ID3D11ShaderResourceView* poolSRV;
	ID3D11ShaderResourceView* poolNormalMapSRV;

	ID3D11SamplerState* sampler;

	// Render targets for deferred rendering.
	ID3D11RenderTargetView* renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthState;

	ID3D11RasterizerState* deferredRasterState;
	ID3D11BlendState* blendState;

	// Buffers to hold geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Wrappers for DirectX shaders
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
	SimpleVertexShader* particleVertexShader;
	SimplePixelShader* particlePixelShader;
	SimpleVertexShader* skyboxVertexShader;
	SimplePixelShader* skyboxPixelShader;
	SimpleVertexShader* waterVertexShader;
	SimplePixelShader* waterPixelShader;
	SimpleVertexShader* refractionVertexShader;
	SimplePixelShader* refractionPixelShader;
	SimpleGeometryShader* backfaceGeometryShader;
	SimpleDomainShader* displacementDomainShader;
	SimpleHullShader* displacementHullShader;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position. For determining how far the mouse moved in a single frame.
	POINT prevMousePos;

public:
	Engine(HINSTANCE hInstance);
	~Engine();

	// Overridden setup and render loop methods, which will be called automatically.
	void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Mouse input helper methods - Change to DX Input
	void MousePressed (WPARAM buttonState, int x, int y);
	void MouseReleased (WPARAM buttonState, int x, int y);
	void MouseMoving (WPARAM buttonState, int x, int y);
};

