#pragma once

#include <WindowsX.h>
#include <d3d11.h>
#include <string>
#include <sstream>

class DXInit
{
private:
	// Timing related data
	double perfCounterSeconds;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 previousTime;

	// FPS calculation
	int fpsFrameCount;
	float fpsTimeElapsed;

	void UpdateTimer();			// Updates the timer for this frame.
	void UpdateTitleStats();	// Puts debug info in the title bar.

protected:
	HINSTANCE	hInstance;		// The handle to the application
	HWND		hWnd;			// The handle to the window itself
	std::string titleBarText;	// Custom text in window's title bar
	bool		titleBarStats;	// Show extra stats in title bar?

	// Size of the window's client area
	unsigned int width;
	unsigned int height;

	// DirectX related objects.
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	D3D_FEATURE_LEVEL dxFeatureLevel;

	// Helper function for allocating a console window
	void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

public:
	DXInit(
		HINSTANCE hInstance,		// The application's handle.
		const char* titleText,		// Text for the window's title bar.
		unsigned int windowWidth,	// Width of the window's client area.
		unsigned int windowHeight,	// Height of the window's client area.
		bool debugTitleStats);		// Show extra stats (fps).
	~DXInit();

	// Static requirements for OS-level message processing
	static DXInit* DXCoreInstance;
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,		// Window handle
		UINT uMsg,		// Message
		WPARAM wParam,	// Message's first parameter
		LPARAM lParam	// Message's second parameter
		);
	
	// Internal method for message handling
	LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Initialization and render loop related methods
	HRESULT InitWindow();
	HRESULT InitDirectX();
	HRESULT Run();				
	void Quit();
	
	// Pure virtual methods for setup and functionality
	virtual void Init()	= 0;
	virtual void Update(float deltaTime, float totalTime) = 0;
	virtual void Draw(float deltaTime, float totalTime) = 0;

	// Convenience methods for handling mouse input - BYTA TILL DX INPUT WAAAAA
	virtual void MousePressed(WPARAM buttonState, int x, int y) { }
	virtual void MouseReleased(WPARAM buttonState, int x, int y) { }
	virtual void MouseMoving(WPARAM buttonState, int x, int y) { }
	
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;
};

