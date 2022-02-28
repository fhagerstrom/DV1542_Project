#include "Defines.h"
#include "DxHandler.h"





bool CreateInterfaces(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& context, IDXGISwapChain*& swapChain)
{
    DXGI_SWAP_CHAIN_DESC desc = {}; 

    desc.BufferDesc.Width = width; 
    desc.BufferDesc.Height = height; 
    desc.BufferDesc.RefreshRate.Numerator = 0; // RefreshRate 60 hertz (0 from the beginning) 
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bits with 8 per channel
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order unspecified
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //  scaling is unspecified
    desc.SampleDesc.Count = 1; // one desc
    desc.SampleDesc.Quality = 0; //default
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //use resource or surface as result of rendering
    desc.BufferCount = 1; //double buffer
    desc.OutputWindow = window;
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //deletes contents of backup buffer when called on
    desc.Flags = 0; 

    UINT flags = 0;

#ifdef _DEBUG
    flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 }; //Targets features supported by Direct3D 11.0, including shader model 5.

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, //IDXGI Adapter
        D3D_DRIVER_TYPE_HARDWARE, // Program selects, since first parameter is nullptr
        nullptr, // driver type of software
        flags, // which runtime layers we enable, which are "flags"
        featureLevels, // pointer to array which determines order of feature levels created. in this case D3D_FEATURE_LEVEL_11_0
        1, //element is called D3D_FEATURE_LEVEL_11_0
        D3D11_SDK_VERSION, // Standard sdk_version used
        &desc, // swapchain desc
        &swapChain, // adress of swapchain
        &device, // adress of device
        nullptr, // no definition needed
        &context// adrress of immidiatecontext
    );

    return !(FAILED(hr));
}

bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv)
{
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer))))
    {
        std::cerr << "Failed to get back buffer!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
    backBuffer->Release();
    return !FAILED(hr);
}

bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView)
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width; 
    desc.Height = height; 
    desc.MipLevels = 1; //no need for multisampled texture, this way renders faster
    desc.ArraySize = 1; // just one picture
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 32-bit z-buffer format which supports 24 bits for depth & 8 bits for stencil.
    desc.SampleDesc.Count = 1; // how many multisamples per pixel
    desc.SampleDesc.Quality = 0; // No need for better quality
    desc.Usage = D3D11_USAGE_DEFAULT; //´the resource gets both read and write access of gpu
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // texture is bound as a depth-stencil target
    desc.CPUAccessFlags = 0; // CPU access not needed
    desc.MiscFlags = 0; // No more flags required

    if (FAILED(device->CreateTexture2D(&desc, nullptr, &dsTexture)))
    {
        std::cerr << "Failed to create depth stencil texture!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);
    return !(FAILED(hr));
}

void SetViewPort(D3D11_VIEWPORT& viewport, UINT width, UINT height)
{
    viewport.TopLeftY = 0;
    viewport.TopLeftX = 0;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MaxDepth = 1;
    viewport.MinDepth = 0;
}

bool CreateGbuffer(ID3D11Device* device, GBuffer& gBuffer)
{

    
    //time to make some textures
    D3D11_TEXTURE2D_DESC TextureDesc; //gbuffer desc
    ZeroMemory(&TextureDesc, sizeof(TextureDesc));

    TextureDesc.Width = gBuffer.screenWidth;
    TextureDesc.Height = gBuffer.screenHeight;
    TextureDesc.MipLevels = 1; // Lowest possible, defaulted is -1 which is impossible
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;  //both render target & Shader resource can write to and use in shader
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    HRESULT hr;

    for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
    {
        hr = device->CreateTexture2D(&TextureDesc, nullptr, &gBuffer.gBufferTexture[i]);
        if (FAILED(hr))
        {
            std::cerr << "ERROR! COULD NOT GREATE GBUFFER TEXTURE2D!" << std::endl;
            return false;
        }
    }

    // Create RTVs
    D3D11_RENDER_TARGET_VIEW_DESC rtvGbufferDesc;
    ZeroMemory(&rtvGbufferDesc, sizeof(rtvGbufferDesc));
    rtvGbufferDesc.Format = TextureDesc.Format; // Same format as texture
    rtvGbufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // using it as texture2D
    rtvGbufferDesc.Texture2D.MipSlice = 0;

    for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
    {
        hr = device->CreateRenderTargetView(gBuffer.gBufferTexture[i], &rtvGbufferDesc, &gBuffer.gBuffergBufferRtv[i]);
        if (FAILED(hr))
        {
            std::cerr << "ERROR! FAILED TO CREATE RTV GBUFFER!" << std::endl;
            return false;
        }
    }


    // time to make shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC desc; //srv desc
    ZeroMemory(&desc, sizeof(desc));
    desc.Format = TextureDesc.Format;// Same format as the texture of course
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.MipLevels = 1;	// Lowest possible (default is -1 once again)

    for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
    {
        hr = device->CreateShaderResourceView(gBuffer.gBufferTexture[i], &desc, &gBuffer.gBufferSrv[i]);
        if (FAILED(hr))
        {
            std::cerr << "ERROR! COUD NOT CREATE SHADER RESOURCE VIEW FOR GBUFFER!" << std::endl;
            return false;
        }
    }
    return true;
}

// Calls the various functions
bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& context,
    IDXGISwapChain*& swapchain, ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, GBuffer& gBuffer)
{
    if (!CreateInterfaces(width, height, window, device, context, swapchain))
    {
        std::cerr << "Error! COULD NOT CREATE INTERFACES!" << std::endl;
        return false;
    }

    // this is for deferred rendering
    if (!CreateGbuffer(device, gBuffer))
    {
        OutputDebugString(L"ERROR! FAILED TO CREATE GBUFFER!");
        return false;
    }

    if (!CreateRenderTargetView(device, swapchain, rtv))
    {
        std::cerr << "ERROR! FAILED TO CREATE RENDER TARGET VIEW" << std::endl;
        return false;
    }

    if (!CreateDepthStencil(device, width, height, dsTexture, dsView))
    {
        std::cerr << "ERROR! FAILED TO CREATE DEPTH STENCIL" << std::endl;
        return false;
    }

    SetViewPort(viewport, width, height);

    return true;
}

