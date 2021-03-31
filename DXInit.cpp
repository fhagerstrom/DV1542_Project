#include "DXInit.h"

DXInit* DXInit::DXInitInstance = 0;

DXInit::DXInit(HINSTANCE hInstance, const char* titleText, int windowWidth, int windowHeight, bool debugStats)
{
	DXInitInstance = this;

	// Save params
	this->hInstance = hInstance;
	this->titleText = titleText;
	this->width = windowWidth;
	this->height = windowHeight;
	this->debugStats = debugStats;

	// Init fields
	fpsFrameCount = 0;
	fpsTimeElapsed = 0;

	device = nullptr;
	context = nullptr;
	swapChain = nullptr;
	backBufferRTV = nullptr;
	depthStencilView = nullptr;
}

DXInit::~DXInit()
{
	// Release DX resources
	if (depthStencilView) { depthStencilView->Release(); }
	if (backBufferRTV) { backBufferRTV->Release(); }
	if (swapChain) { swapChain->Release(); }
	if (context) { context->Release(); }
	if (device) { device->Release(); }
}

HRESULT DXInit::InitWindow()
{
	const wchar_t CLASS_NAME[] = L"3D Project Win Class";

	WNDCLASS winClass = {}; // Zero out memory

	winClass.style = CS_HREDRAW | CS_VREDRAW;	// Redraw on horizontal or vertical movement/adjustment
	winClass.lpfnWndProc = DXInit::WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;	// Our app's handle
	winClass.lpszClassName = CLASS_NAME;

	RegisterClass(&winClass);

	hWnd = CreateWindowEx(0, CLASS_NAME, L"3D Project", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInstance, nullptr);

	if (hWnd == NULL)
	{
		DWORD error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

	ShowWindow(hWnd, SW_SHOW);


	return S_OK;

}

HRESULT DXInit::InitDirectX()
{
	unsigned int deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)

	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif

	// SWAPCHAIN DESC
	DXGI_SWAP_CHAIN_DESC swapchainDesc = {};
	swapchainDesc.BufferCount = 1;
	swapchainDesc.BufferDesc.Width = width;
	swapchainDesc.BufferDesc.Height = height;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 120;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Align 8 bits per channel - Unsigned normalized
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Default
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Default

	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.Flags = 0;
	swapchainDesc.OutputWindow = hWnd;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchainDesc.Windowed = true;

	HRESULT hr = S_OK;

	dxFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	hr = D3D11CreateDeviceAndSwapChain(
		0,							// Video adapter (physical GPU) to use, or null for default
		D3D_DRIVER_TYPE_HARDWARE,	// We want to use the GPU
		0,							
		deviceFlags,				// Special options
		0,							// Optional array of possible verisons we want as fallbacks
		0,							// The number of fallbacks in the above param
		D3D11_SDK_VERSION,			// Current version of the SDK
		&swapchainDesc,				// Address of swap chain options
		&swapChain,					// Pointer to our Swap Chain pointer
		&device,					// Pointer to our Device pointer
		&dxFeatureLevel,			// This will hold the actual feature level the app will use
		&context);					// Pointer to our Device Context pointer

	if (FAILED(hr)) 
		return hr;

	// BACKBUFFER AND DEPTH STENCIL
	ID3D11Texture2D* backBufferTexture;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);

	device->CreateRenderTargetView(backBufferTexture, 0, &backBufferRTV);
	backBufferTexture->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* depthBufferTexture;
	device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	device->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
	depthBufferTexture->Release();

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

	// VIEWPORT
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	// Return "Everything is ok" HRESULT value
	return S_OK;
}

HRESULT DXInit::Run()
{
	// Grab start time
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now); // Time-stamp function

	startTime = now;
	currentTime = now;
	previousTime = now;

	// Init subclass
	Init();

	// Overall message and window loop
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			// Update timer and title (if necessary)
			UpdateTimer();

			if (debugStats)
				UpdateTitleBarStats();

			// Update and draw
			// Update(deltaTime, totalTime);
			// Draw(deltaTime, totalTime);
		}
		
	}

	return msg.wParam;
}

void DXInit::UpdateTimer()
{
	// Grab the current time
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	currentTime = now;

	// Calculate delta time and clamp to zero
	//  - Could go negative if CPU goes into power save mode 
	//    or the process itself gets moved to another core
	deltaTime = max((float)((currentTime - previousTime) * perfCounterSeconds), 0.0f);

	// Calculate the total time from start to now
	totalTime = (float)((currentTime - startTime) * perfCounterSeconds);

	// Save current time for next frame
	previousTime = currentTime;
}

// Updates window title bar with stats such as
// - Window width and height
// - FPS and frame
// - DX version
void DXInit::UpdateTitleBarStats()
{
	fpsFrameCount++;

	// Calculating FPS and update once per second only
	float timeDifference = totalTime - fpsTimeElapsed;
	if (timeDifference < 1.0f)
		return;

	// Title bar text (Display info)
	std::ostringstream output;
	output.precision(6);
	output << titleText << "	Width: " << width << "	 Height: " << height << "	FPS: " << fpsFrameCount << "	DX " << dxFeatureLevel;

	// Apply update on title and reset FPS data
	
	SetWindowTextA(hWnd, output.str().c_str());
	fpsFrameCount = 0;
	fpsTimeElapsed += 1.0f;
}

// Create a cmd window for debugging purposes
//
// bufferLines - Number of lines in the overall console buffer
// bufferColumns - Numbers of columns in the overall console buffer
// windowLines   - Number of lines visible at once in the window
// windowColumns - Number of columns visible at once in the window
void DXInit::CreateCmdWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = bufferLines;
	coninfo.dwSize.X = bufferColumns;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = windowColumns;
	rect.Bottom = windowLines;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	FILE* stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent accidental console window close
	HWND consoleHandle = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

LRESULT DXInit::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Switch case that checks if the window instance is closed or not
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


LRESULT DXInit::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DXInitInstance->ProcessMessage(hWnd, uMsg, wParam, lParam);
}