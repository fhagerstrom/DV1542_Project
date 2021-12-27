#pragma once
#include "Particles.h"
#include "DirectXHelper.h"
#include "Shaders.h"

class RenderParticles
{
private:
	unsigned int stride = sizeof(XMFLOAT3);
	unsigned int offset = 0;

	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* viewPersPectiveBuffer;
	const std::string psPath = "x64/Debug/ParticlePS.cso";
	ID3D11PixelShader* pixelShader;
	const std::string gsPath = "x64/Debug/ParticleGS.cso";
	ID3D11GeometryShader* geometryShader;

public:
	RenderParticles(ID3D11Device* device);
	void Render(ID3D11DeviceContext* context, Particles* particles);
	void ShutDown();
};