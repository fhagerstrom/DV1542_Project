#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class DxHandler
{
public:
	DxHandler();
	DxHandler(const DxHandler&);
	~DxHandler();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	/* GET FUNCTIONS */
	ID3D11Device* GetDevice() { return devicePtr; }
	ID3D11DeviceContext* GetDeviceContext() { return this->contextPtr; }
	IDXGISwapChain* GetSwapChain() { return this->swapChain; }
	ID3D11Texture2D* GetBackBuffer() { return this->backBufferPtr; }
	ID3D11SamplerState* GetSampler() { return this->sampler; }
	ID3D11BlendState* GetBlendState() { return this->blendState; }
	ID3D11RenderTargetView* GetRtv() { return this->renderTargetView; }
	ID3D11DepthStencilView* GetDsv() { return this->depthStencilView; }

	void GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = this->projectionMatrix; }
	void GetWorldMatrix(XMMATRIX& worldMatrix) { worldMatrix = this->worldMatrix; }

private:
	IDXGISwapChain* swapChain;
	ID3D11Device* devicePtr;
	ID3D11DeviceContext* contextPtr;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11BlendState* blendState;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11Texture2D* backBufferPtr;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;

	ID3D11SamplerState* sampler;

	XMMATRIX projectionMatrix;
	XMMATRIX worldMatrix;

	// TODO: RENAME VARIABLES
};