#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class DxHandler
{
public:
	DxHandler();
	DxHandler(const DxHandler&);
	~DxHandler();

	bool Initialize(int, int, HWND, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	/* GET FUNCTIONS */
	ID3D11SamplerState* GetMinMagMipSampler() { return this->minMagMipSampler; }

	void GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = this->projectionMatrix; }
	void GetWorldMatrix(XMMATRIX& worldMatrix) { worldMatrix = this->worldMatrix; }
	void GetOrthoMatrix(XMMATRIX& orthoMatrix) { orthoMatrix = this->orthoMatrix; }

private:
	IDXGISwapChain* swapChain;
	ID3D11Device* devicePtr;
	ID3D11DeviceContext* contextPtr;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;

	ID3D11SamplerState* minMagMipSampler;

	XMMATRIX projectionMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;

	// TODO: RENAME VARIABLES
};