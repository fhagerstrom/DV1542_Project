#pragma once
#include "dinput.h"


class Input
{
public:
	Input(HWND hwnd);
	~Input();

	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	void Shutdown();

	IDirectInput8* GetDirectInput8() { return directInput; }
	IDirectInputDevice8* GetDirectInput8Keyboard() { return keyboard; }
	IDirectInputDevice8* GetDirectInput8Mouse() { return mouse; }
	DIMOUSESTATE GetlastState() { return this->mouseState; }

private:
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char keyboardState[256];
	DIMOUSESTATE mouseState;
	HWND hwnd;
};
