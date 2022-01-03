#pragma once
#include "DxHandler.h"
#include "Camera.h"
#include "Shadow.h"
#include "Lighting.h"

class Shaders
{
	// A friend class can access private and protected members of another class in which it is declared as friend
	friend class RenderParticles;
	friend class RenderModels;
	friend class RenderShadows;
	friend class Tesselation;

private:
	static std::string positionOnlyVSPath;
	static ID3D11InputLayout* positionOnlyLayout;	// positionOnlyLayout used for particle and shadow renderering
	static ID3D11VertexShader* positionOnlyVS;
	static std::string gsPath; 					// For GeometryShader 

	static ID3D11InputLayout* modelLayout;			// model_layout used for model

	// Setting this in update, and getting the values from Camera and DirectionalLight
	static XMFLOAT3 camPosition;					// XMFLOAT3 containg the cameras Position
	static XMMATRIX viewMatrix;						// Matrix containing the cameras viewMatrix
	static XMMATRIX perspectiveMatrix;				// Matrix containing the cameras perpectiveMatrix
	static XMMATRIX lightingMatrix;					// Matrix containing the light orthographic viewProjection-matrix
	//static ID3D11Buffer* camPos; 				// Used for the backfaced culling

public:
	static ID3D11GeometryShader* gShader;
	static Shadow* shadowmap;
	static void Shutdown();
	static void Initialize(ID3D11Device* device, std::string modelVSByteCode);
	static void Update(ID3D11DeviceContext*& context, Camera camera, Lighting& light);
};