#include "DXInit.h"

DXInit* DXInit::DXCoreInstance = 0;

LRESULT DXInit::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DXCoreInstance->ProcessMessage(hWnd, uMsg, wParam, lParam);
}


DXInit::DXInit(
	HINSTANCE hInstance, // The application's handle.
	const char* titleText,	// Text for the window's title bar.
	unsigned int windowWidth, // Width of the window's client area.
	unsigned int windowHeight, // Height of the window's client area.
	bool debugTitleStats)	// Show extra stats (fps).
{
	DXCoreInstance = this;

	// Save params
	this->hInstance = hInstance;
	this->titleBarText = titleText;
	this->width = windowWidth;
	this->height = windowHeight;
	this->titleStats = debugTitleStats;

	// Initialize fields.
	fpsFrameCount = 0;
	fpsTimeElapsed = 0.0f;
	
	device = 0;
	context = 0;
	swapChain = 0;
	backBufferRTV = 0;
	depthStencilView = 0;

	// Query performance counter for accurate timing information
	__int64 perfFreq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
	perfCounterSeconds = 1.0 / (double)perfFreq;
}

DXInit::~DXInit()
{
	// Release all DirectX resources.
	if (depthStencilView) { depthStencilView->Release(); }
	if (backBufferRTV) { backBufferRTV->Release(); }
	if (swapChain) { swapChain->Release(); }
	if (context) { context->Release(); }
	if (device) { device->Release(); }
	
}

HRESULT DXInit::InitWindow()
{
	const wchar_t CLASS_NAME[] = L"3D Project Class";

	WNDCLASS winClass = {}; // Zero out the memory

	winClass.lpfnWndProc = DXInit::WindowProc;
	winClass.hInstance = hInstance;	// Our app's handle
	winClass.lpszClassName = CLASS_NAME;

	RegisterClass(&winClass);

	if (!RegisterClass(&winClass))
	{	
		DWORD error = GetLastError();

		if (error != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(error);
	}

	hWnd = CreateWindowEx(0, CLASS_NAME, L"3D Project Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInstance, nullptr);

	
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
	// This will hold options for DirectX initialization
	unsigned int deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true;


	HRESULT hr = S_OK;

	hr = D3D11CreateDeviceAndSwapChain(
		0,							// Video adapter (physical GPU) to use, or null for default
		D3D_DRIVER_TYPE_HARDWARE,	// We want to use the hardware (GPU)
		0,							// Used when doing software rendering
		deviceFlags,				// Any special options
		0,							// Optional array of possible verisons we want as fallbacks
		0,							// The number of fallbacks in the above param
		D3D11_SDK_VERSION,			// Current version of the SDK
		&swapDesc,					// Address of swap chain options
		&swapChain,					// Pointer to our Swap Chain pointer
		&device,					// Pointer to our Device pointer
		&dxFeatureLevel,			// This will hold the actual feature level the app will use
		&context);					// Pointer to our Device Context pointer
	if (FAILED(hr)) return hr;

	
	ID3D11Texture2D* backBufferTexture;
	swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&backBufferTexture);

	device->CreateRenderTargetView(
		backBufferTexture,
		0,
		&backBufferRTV);
	backBufferTexture->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width				= width;
	depthStencilDesc.Height				= height;
	depthStencilDesc.MipLevels			= 1;
	depthStencilDesc.ArraySize			= 1;
	depthStencilDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags		= 0;
	depthStencilDesc.MiscFlags			= 0;
	depthStencilDesc.SampleDesc.Count	= 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* depthBufferTexture;
	device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	device->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
	depthBufferTexture->Release();

	
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	return S_OK;
}

//HRESULT DXInit::InitDirectInput(HINSTANCE hInstance)
//{
//	HRESULT hr = S_OK;
//
//	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
//
//	hr = directInput->CreateDevice(GUID_SysKeyboard,
//		&DIKeyboard,
//		NULL);
//
//	hr = directInput->CreateDevice(GUID_SysMouse,
//		&DIMouse,
//		NULL);
//
//	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
//	hr = DIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
//
//	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
//	hr = DIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
//
//	return S_OK;
//}

HRESULT DXInit::Run()
{
	// Grab the start time now, when the loop is running.
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	startTime = now;
	currentTime = now;
	previousTime = now;

	// Give subclass a chance to initialize
	Init();

	// Our overall game and message loop
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Determine if there is a message waiting
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate and dispatch the message to WindowProc.
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update timer and title bar if necessary.
			UpdateTimer();
			if(titleStats)
				UpdateTitleStats();

			// The render loop.
			Update(deltaTime, totalTime);
			Draw(deltaTime, totalTime);
		}
	}

	// We'll end up here once we get a WM_QUIT message, usually when closing the window
	return msg.wParam;
}

// Sends an OS-level Quit message to our process
void DXInit::Quit()
{
	PostQuitMessage(0);
}

// Uses high resolution time stamps to get timing information, and calculates useful time stats.
void DXInit::UpdateTimer()
{
	// Grab the current time.
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	currentTime = now;

	// Calculate delta time and clamp to zero
	deltaTime = max((float)((currentTime - previousTime) * perfCounterSeconds), 0.0f);

	// Calculate the total time from start to now
	totalTime = (float)((currentTime - startTime) * perfCounterSeconds);

	// Save current time for next frame
	previousTime = currentTime;
}

// Updates the window's title bar with several stats once
// per second, including:
//  - The window's width & height
//  - The current FPS and ms/frame
//  - The version of DirectX being used
void DXInit::UpdateTitleStats()
{
	fpsFrameCount++;

	// Only calc FPS and update title bar once per second
	float timeDiff = totalTime - fpsTimeElapsed;
	if (timeDiff < 1.0f)
		return;

	// Calculate how long a frame took approximately
	float mspf = 1000.0f / (float)fpsFrameCount;

	// Dirty title bar text.
	std::ostringstream output;
	output.precision(6);
	output << titleBarText << "    Width: " << width << "    Height: " << height << "    FPS: " << fpsFrameCount << "    Frame Time: " << mspf << "ms    " << "   DX 11.0";

	// Update the title bar and reset fps data.
	SetWindowTextA(hWnd, output.str().c_str());
	fpsFrameCount = 0;
	fpsTimeElapsed += 1.0f;
}

// Allocates a console window we can print to for debugging
// 
// bufferLines   - Number of lines in the overall console buffer
// bufferColumns - Numbers of columns in the overall console buffer
// windowLines   - Number of lines visible at once in the window
// windowColumns - Number of columns visible at once in the window
void DXInit::CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
{
	// Temporary console info struct
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// Get the console info and set the number of lines
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

	FILE *stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent closing of the console window.
	HWND consoleHandle = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

// Handles messages that are sent to our window.
LRESULT DXInit::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Check the incoming message and handle any we care about.
	switch (msg)
	{
	// This is the message that signifies the window closing
	case WM_DESTROY:
		PostQuitMessage(0); // Send a quit message to our own program
		return 0;

	// Mouse button being pressed (while the cursor is currently over our window)
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		MousePressed(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	// Mouse button being released (while the cursor is currently over our window)
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		MouseReleased(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	// Cursor moves over the window (or outside, while we're currently capturing it)
	case WM_MOUSEMOVE:
		MouseMoving(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	// Let Windows handle any messages we're not touching
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

