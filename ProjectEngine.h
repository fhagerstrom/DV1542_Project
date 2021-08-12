#pragma once
const int BUFFER_COUNT = 3;

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
#include "ShadowMapping.h"
#include "WICTextureLoader.h" // DirectXTK

class Engine : public DXInit
{
private:
	// Input and mesh swapping.
	unsigned int currentEntity;

	// Keep track of how many times player have flown through ring.
	int currentScore;

	// switch counter for changing buffer views for deferred
	int switcher;

	// Water related float variable for "translation"
	float waterTranslation;
	
	// Displacement mapping.
	float tessellationAmount;
	float HScale;
	float HBias;

	// Initialization helper functions.
	void SetDeferred(int textureWidth, int textureHeight);
	void CreateMatrices();
	void LoadShaders();
	void LoadModels();
	void LoadTextures();
	void CreateMaterials();
	void CreateWater();
	void CreateEntities();
	void LightEntityInitialize();
	void CreateShadows();
	void RenderShadowMap();

	// Texture related schtuff.
	ID3D11ShaderResourceView* objectTextureSRV;
	ID3D11ShaderResourceView* objectNormalMapSRV;
	ID3D11ShaderResourceView* planeTextureSRV;
	ID3D11ShaderResourceView* planeNormalMapSRV;
	ID3D11ShaderResourceView* waterSRV;
	ID3D11ShaderResourceView* waterNormalMapSRV;
	ID3D11ShaderResourceView* poolSRV;
	ID3D11ShaderResourceView* poolNormalMapSRV;

	ID3D11SamplerState* sampler;

	// Deferred rendering objects and settings.
	ID3D11RenderTargetView* renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthState;

	ID3D11RasterizerState* deferredRasterState;
	ID3D11BlendState* blendState;
	D3D11_VIEWPORT viewport;

	SimpleVertexShader* deferredVertexShader;
	SimplePixelShader* deferredPixelShader;
	SimpleVertexShader* lightVertexShader;
	SimplePixelShader* lightPixelShader;

	SimpleVertexShader* backBufferVertexShader;
	SimplePixelShader* backBufferPixelShader;

	// Wrappers for DirectX shaders
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* dirVertexShader;
	SimplePixelShader* dirPixelShader;
	SimpleVertexShader* particleVertexShader;
	SimplePixelShader* particlePixelShader;
	SimpleVertexShader* waterVertexShader;
	SimplePixelShader* waterPixelShader;
	SimpleGeometryShader* backfaceGeometryShader;
	SimpleDomainShader* displacementDomainShader;
	SimpleHullShader* displacementHullShader;

	// Buffers to hold geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Shadow schtuff
	int shadowMapSize;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVertexShader;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	// Keep track of objects and entities.
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

	// Mesh objects.
	Mesh* objectMesh;
	Mesh* ringMesh;
	Mesh* planeMesh;
	Mesh* lightMesh;
	Mesh* poolMesh;
	Mesh* waterMesh;

	Water* water;

	// Material class objects.
	Material* objectMaterial;
	Material* ringMaterial;
	Material* planeMaterial;
	Material* poolMaterial;
	Material* waterMaterial;

	Renderer renderer;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position. For determining how far the mouse moved in a single frame.
	POINT prevMousePos;

public:
	Engine(HINSTANCE hInstance);
	~Engine();

	// Overridden setup and Renderer loop methods, which will be called automatically.
	void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Mouse input helper methods - Change to DX Input
	void MousePressed (WPARAM buttonState, int x, int y);
	void MouseReleased (WPARAM buttonState, int x, int y);
	void MouseMoving (WPARAM buttonState, int x, int y);
};

