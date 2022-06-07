#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Input.h"
#include "Graphics.h"
#include "Timer.h"

class SystemHandler
{
public:
	SystemHandler();
	~SystemHandler();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR appName;
	HINSTANCE hInstance;
	HWND hwnd;

	Input* input;
	Graphics* graphics;
	Timer frameTimer;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemHandler* applicationHandle = 0;
