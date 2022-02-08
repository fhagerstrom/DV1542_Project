#include "Defines.h"
#include"Shaders.h"

std::string Shaders::positionOnlyVSPath = "Debug/ParticleVS.cso";
ID3D11VertexShader* Shaders::positionOnlyVS;

XMFLOAT3 Shaders::camPosition;
XMMATRIX Shaders::viewMatrix;
XMMATRIX Shaders::lightingMatrix;
XMMATRIX Shaders::projectionMatrix;

ID3D11InputLayout* Shaders::positionOnlyLayout;
ID3D11InputLayout* Shaders::modelLayout;

ID3D11GeometryShader* Shaders::gShader;
ID3D11HullShader* Shaders::hShader;
ID3D11DomainShader* Shaders::dShader;

Shadow* Shaders::shadowmap;

void Shaders::Shutdown()
{
	if(positionOnlyLayout)
		positionOnlyVS->Release();
	if(positionOnlyLayout)
		positionOnlyLayout->Release();
	if(modelLayout)
		modelLayout->Release();
	//camPos->Release();
	if(gShader)
		gShader->Release();
	if(shadowmap)
		shadowmap->ShutDown();
}

void Shaders::Initialize(ID3D11Device* device, std::string modelVSByteCode)
{
	shadowmap = new Shadow(device, 4096);
	std::string byteCode;
	std::string shaders;
	std::ifstream reader;
	// Clears the reader and Shaders and does the same thing for the PixelShader

	reader.open(positionOnlyVSPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return;
	}

	reader.seekg(0, std::ios::end);
	shaders.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaders.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaders.c_str(), shaders.length(), nullptr, &positionOnlyVS)))
	{
		std::cerr << "Failed to create position VERTEX shader!" << std::endl;
		return;
	}

	byteCode = shaders;

	shaders.clear();
	reader.close();

	D3D11_INPUT_ELEMENT_DESC positionDesc[1] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(positionDesc, 1, byteCode.c_str(), byteCode.length(), &positionOnlyLayout);
	if (FAILED(hr))
	{
		std::cout << "FAILED TO CREATE POSITION INPUT LAYOUT" << std::endl;
	}

	// Input layout, array element description
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		// Name, index, format, slot, byteOffset, inputSlotClass, stepRate
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Input data is per-vertex data.
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Viktigt!! Allt behöver vara i rätt ordning för om det laddas in i fel ordning så funkar det ej
	hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), modelVSByteCode.c_str(), modelVSByteCode.length(), &modelLayout);

	if FAILED(hr)
	{
		std::cerr << "ERROR INPUTLAYOUT" << std::endl;
		return;
	}

	shaders.clear();
	reader.close();
}


void Shaders::Update(ID3D11DeviceContext*& context, Camera camera, Lighting& light)
{
	camPosition = camera.GetPosition();
	viewMatrix = camera.GetViewMatrix();
	projectionMatrix = camera.GetProjectionMatrix();
	lightingMatrix = light.GetMatrix();	// Lights transposed orthographic viewPerspective matrix

	// CAM POS
	// std::cout << "X: " << cameraPosition.x << " Y: " << cameraPosition.y << " Z: " << cameraPosition.z << std::endl;
}
