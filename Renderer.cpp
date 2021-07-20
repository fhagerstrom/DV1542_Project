#include "Renderer.h"

namespace dx =  DirectX;

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::SetLights()
{
	pointLight1.SetpointLightValues(dx::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f), dx::XMFLOAT3(3, 0, 0));
	pointLight2.SetpointLightValues(dx::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f), dx::XMFLOAT3(-3, 0, 0));
	pointLight3.SetpointLightValues(dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), dx::XMFLOAT3(-1, 1, 0));
	pointLight4.SetpointLightValues(dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), dx::XMFLOAT3(1, 1, 0));

	spotLight1.SetSpotLightValues(dx::XMFLOAT3(0, 1, 0), 1.0f);
	spotLight2.SetSpotLightValues(dx::XMFLOAT3(0, 1, 0), 1.0f);
}

 