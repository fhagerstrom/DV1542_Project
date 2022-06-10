#include "SystemHandler.h"

SystemHandler::SystemHandler()
{
	input = 0;
	graphics = 0;
}

SystemHandler::~SystemHandler()
{
}


bool SystemHandler::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen.
	screenWidth = 1280;
	screenHeight = 720;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object. This object will be used to handle reading the keyboard input from the user.
	input = new Input;
	if(!input)
	{
		return false;
	}

	// Initialize the input object.
	input->Initialize();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	graphics = new Graphics;
	if(!graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = graphics->Initialize(screenWidth, screenHeight, hwnd);
	if(!result)
	{
		return false;
	}
	
	return true;
}


void SystemHandler::Shutdown()
{
	// Release the graphics object.
	if(graphics)
	{
		graphics->Shutdown();
		delete graphics;
		graphics = 0;
	}

	// Release the input object.
	if(input)
	{
		delete input;
		input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
	
	return;
}


void SystemHandler::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}

		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}


bool SystemHandler::Frame()
{
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if(input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	frameTimer.Frame();

	// Do the frame processing for the graphics object.
	result = graphics->Frame(frameTimer.DeltaTime());
	if(!result)
	{
		MessageBox(NULL, L"Couldn't process frame for graphics", L"SYSTEMHANDLER ERROR", MB_OK);
		return false;
	}

	return true;
}


LRESULT CALLBACK SystemHandler::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


void SystemHandler::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;

	// Get an external pointer to this object.	
	applicationHandle = this;

	// Get the instance of this application.
	hInstance = GetModuleHandle(NULL);

	// Give the application a name.
	appName = L"3D Project Thingy";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon		 = NULL;
	wc.hIconSm       = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = appName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Resizes rect to set screen size.
	RECT windowRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);


	hwnd = CreateWindowEx(0,
		appName,
		appName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	assert(hwnd);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowCursor(true);

	return;
}


void SystemHandler::ShutdownWindows()
{
	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(appName, hInstance);
	hInstance = NULL;

	// Release the pointer to this class.
	applicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler.
		default:
		{
			return applicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}