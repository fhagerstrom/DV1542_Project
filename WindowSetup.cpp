#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "WindowSetup.h"



WindowSetup::WindowSetup()
{

}


WindowSetup::WindowSetup(const WindowSetup& other)
{

}

WindowSetup::~WindowSetup()
{

}

bool WindowSetup::Initialize()
{
	int width = 0;
	int height = 0;
	bool result;

	// Initialize windows
	InitWindow(width, height);

	application = new EngineAndScene;
	if (!application)
	{
		std::cerr << "Application not initialized!" << std::endl;
		return false;
	}

	result = application->Initialize(hInstance, hwnd, width, height);
	if (!result)
	{
		std::cerr << "Window not initialized!" << std::endl;
		return false;
	}

	return true;
}


void WindowSetup::Run()
{
	MSG msg;
	bool done = false;
	bool result;

	// Initialize message structure
	ZeroMemory(&msg, sizeof(MSG));

	while (!done)
	{
		// Handle window messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Quit window if called
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing
			result = Frame();
			if (!result)
			{
				std::cout << "Frame not processed" << std::endl;
				done = true;
			}
		}
	}

	return;
}

bool WindowSetup::Frame()
{
	bool result = application->Frame();
	if (!result)
	{
		std::cerr << "Frame processing is not not working!" << std::endl;
		return false;
	}

	return true;
}

LRESULT CALLBACK WindowSetup::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void WindowSetup::InitWindow(int& width, int& height)
{
	width = 800;
	height = 600;

	WNDCLASS winClass = {};

	// External pointer to object
	windowHandler = this;

	// Get instance
	hInstance = GetModuleHandle(NULL);

	windowName = L"Project Engine";

	winClass.lpfnWndProc = WndProc; // Window procedure pointer
	winClass.hInstance = hInstance; // Set instance
    winClass.lpszClassName = windowName; // String identifier

	// Register window class
	RegisterClass(&winClass);

	hwnd = CreateWindowEx(0, windowName, L"3D Project", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInstance, nullptr);

	return;

}

void WindowSetup::ReleaseObjects()
{
	DestroyWindow(hwnd);
	hwnd = NULL;
	hInstance = NULL;
	windowHandler = NULL;
	return;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Check if window is being shutdown
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return windowHandler->MessageHandler(hwnd, message, wParam, lParam);
		}
	}
}
