#pragma once
#include <Windows.h>

#include "EngineAndScene.h"

class WindowSetup
{
private:

	LPCWSTR windowName;
	HINSTANCE hInstance;
	HWND hwnd;
	EngineAndScene* application;

	bool Frame();
	void InitWindow(int&, int&);
	void QuitWindow();

public:
	WindowSetup();
	WindowSetup(const WindowSetup&);
	~WindowSetup();

	bool Initialize();
	void ReleaseObjects();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static WindowSetup* windowHandler = 0;