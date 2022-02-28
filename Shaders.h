#pragma once
#include "DxHandler.h"
#include "Camera.h"
#include "Shadow.h"
#include "Lighting.h"
#include "RenderTessellated.h"

class Shaders
{
	// A friend class can access private and protected members of another class in which it is declared as friend
	friend class RenderParticles;
	friend class RenderModels;
	friend class RenderShadows;
	friend class RenderTessellated;

private:
	static std::string positionOnlyVSPath;
	static ID3D11InputLayout* positionOnlyLayout;	// positionOnlyLayout used for particle and shadow rendering
	static ID3D11VertexShader* positionOnlyVS;

	inline static std::string gsPath; 					// For Geometry shader
	inline static std::string dsPath;					// For Domain shader
	inline static std::string hsPath;					// For Hull shader

	static ID3D11InputLayout* modelLayout;			// Input layout used for model
	static ID3D11InputLayout* tessellModelLayout;			// Input layout used for tessellated model

	// Setting this in update, and getting the values from Camera and DirectionalLight
	static XMFLOAT3 camPosition;					// XMFLOAT3 containg the cameras Position
	static XMMATRIX viewMatrix;						// Matrix containing the cameras viewMatrix
	static XMMATRIX projectionMatrix;				// Matrix containing the cameras perpectiveMatrix
	static XMMATRIX lightingMatrix;					// Matrix containing the light orthographic viewProjection-matrix

public:
	static ID3D11GeometryShader* gShader;
	static ID3D11HullShader* hShader;
	static ID3D11DomainShader* dShader;

	static Shadow* shadowmap;
	static void Shutdown();
	static void Initialize(ID3D11Device* device, std::string modelVSByteCode);
	static void Update(ID3D11DeviceContext*& context, Camera camera, Lighting& light);
};