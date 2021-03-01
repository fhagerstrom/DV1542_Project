#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include <iostream>

class Input
{
public:
	Input();
	Input(const Input&);
	~Input();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	//void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	//bool ReadMouse();
	void ProcessInput();

private:

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	//IDirectInputDevice8* mouse;

	unsigned char keyboardState[256];
	//DIMOUSESTATE mouseState;

	int screenWidth, screenHeight;
	//int mouseX, mouseY;
};
