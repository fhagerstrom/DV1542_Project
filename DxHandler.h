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

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

private:
	IDXGISwapChain* swapChain;
	ID3D11Device* devicePtr;
	ID3D11DeviceContext* contextPtr;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	// TODO: RENAME VARIABLES
};