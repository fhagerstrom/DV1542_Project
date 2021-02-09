#include "D3D11Setup.h"

bool CreateInterface(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; // Refreshrate numerator and denominator specifies how often it should update
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // Refreshrate numerator and denominator specifies how often it should update
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // 8 bits per channel, UNORM means unsigned normalized
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Default value, read about this
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Default value

    // Default multisampling
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    // Specify flags during compile time
    UINT flags = 0;

    if (_DEBUG)
        flags = D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL featureLvls[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLvls, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &immediateContext);

    return !FAILED(hr);
}

bool CreateRTV(ID3D11Device* device, ID3D11RenderTargetView*& rtv, IDXGISwapChain*& swapChain)
{
    ID3D11Texture2D* backBuffer = nullptr;

    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
    {
        std::cerr << "Back buffer failed!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
    backBuffer->Release();

    return !FAILED(hr);
}

bool CreateStencilView(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView)
{
    D3D11_TEXTURE2D_DESC textureDesc;

    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // D24 - 24 bits to depth unsigned normalized, S8 - 8 bits to stencil unsigned int
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT; // Default usage access. Can specify to hardware
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0; // A depth buffer for the CPU does not need to write or read
    textureDesc.MiscFlags = 0;

    if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &dsTexture)))
    {
        std::cerr << "Depth stencil texture failed!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);

    return !FAILED(hr);
}

void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height)
{
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
}


bool SetupD3D11(const UINT width, const UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext,
    ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, IDXGISwapChain*& swapChain, D3D11_VIEWPORT& viewport)
{
    if (!CreateInterface(width, height, window, device, immediateContext, swapChain))
    {
        std::cerr << "Couldn't create interface!" << std::endl;
        return false;
    }

    if (!CreateRTV(device, rtv, swapChain))
    {
        std::cerr << "Couldn't create render target view!" << std::endl;
        return false;
    }

    if (!CreateStencilView(device, width, height, dsTexture, dsView))
    {
        std::cerr << "Couldn't create stencil view!" << std::endl;
        return false;
    }

    SetViewport(viewport, width, height);

    return true;
}
