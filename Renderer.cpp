#include "Renderer.h"

namespace dx =  DirectX;

Renderer::~Renderer()
{
}

void Renderer::SetLights()
{
	pointLight1.SetpointLightValues(dx::XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0), dx::XMFLOAT3(-3, 0, 0));
	pointLight2.SetpointLightValues(dx::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f	), dx::XMFLOAT3(3, 0, 0));
	pointLight3.SetpointLightValues(dx::XMFLOAT4(0.3f, 0.0f, 1.0f, 1.0), dx::XMFLOAT3(-1, 1, 0));
	pointLight4.SetpointLightValues(dx::XMFLOAT4(0.3f, 0.0f, 0.7f, 1.0f), dx::XMFLOAT3(1, 1, 0));

	spotLight1.SetSpotLightValues(dx::XMFLOAT3(2, 2, -2), 0.5);
	spotLight2.SetSpotLightValues(dx::XMFLOAT3(-2, 2, -5), 0.5);
}

 