#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <sstream>

class DXInit
{
private:
	// Timing data
	double perfCounterSeconds;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 previousTime;

	// FPS calculation
	int fpsFrameCount;
	float fpsTimeElapsed;

	void UpdateTimer();			// Updates the timer for this frame
	void UpdateTitleBarStats();	// Debug info in the title bar

protected:
	HINSTANCE	hInstance;		// The handle to the application
	HWND		hWnd;			// The handle to the window itself
	std::string titleText;	// Custom text in window's title bar
	bool debugStats;	// Show extra stats in title bar

	// Window size
	unsigned int width;
	unsigned int height;

	// DirectX related objects and variables
	D3D_FEATURE_LEVEL dxFeatureLevel;
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;

	// Helper function for allocating a console window
	void CreateCmdWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

public:
	DXInit(HINSTANCE hInstance, const char* titleText, int windowWidth, int windowHeight, bool debugStats);
	~DXInit();

	static DXInit* DXInitInstance;
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 

	// Internal message handling method
	LRESULT ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Init and window loop-related methods
	HRESULT InitWindow();
	HRESULT InitDirectX();
	HRESULT Run();

	void Quit();

	// Pure virtual methods for setup and functionality
	virtual void Init() = 0;
	virtual void Update(float deltaTime, float totalTime) = 0;
	virtual void Draw(float deltaTime, float totalTime) = 0;
};