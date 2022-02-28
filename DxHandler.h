#pragma once



enum class shaders { DS, PS };

//Gbuffer to store info about geometry
struct GBuffer
{
	static const int NROFBUFFERS = 8; // How many buffers to use
	int screenWidth;
	int screenHeight;

	//To make Texture, RTV and SRV arrays
	ID3D11Texture2D* gBufferTexture[NROFBUFFERS];
	ID3D11RenderTargetView* gBuffergBufferRtv[NROFBUFFERS];
	ID3D11ShaderResourceView* gBufferSrv[NROFBUFFERS];
};

// standard update buffer function which can map, unmap and specify buffers & data needed
template<typename T>
inline void UpdateBuffer(ID3D11DeviceContext* context, ID3D11Buffer*& buffer, const T& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubResource = {};

	HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	if FAILED(hr)
	{
		std::cerr << "ERROR! FAILED TO UPDATE BUFFER!" << std::endl;
		return;
	}
	memcpy(mappedSubResource.pData, &data, sizeof(data));
	context->Unmap(buffer, 0);
}

//standard create buffer which specifies what buffer to use and what size it should be
inline void CreateBuffer(ID3D11Device* device, ID3D11Buffer*& buffer, unsigned int size)
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.ByteWidth = size;

	HRESULT hr = device->CreateBuffer(&desc, nullptr, &buffer);
	if FAILED(hr)
	{
		std::cerr << "ERROR! FAILED TO CREATE BUFFER!" << std::endl;
	}
}

// Setup all things D3D11
bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device,
	ID3D11DeviceContext*& context, IDXGISwapChain*& swapChain,
	ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture,
	ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, GBuffer& gBuffer);
