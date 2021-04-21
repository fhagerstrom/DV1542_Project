#pragma once
#include <dinput.h>

class Input
{
private:
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char keyboardState[256];
	DIMOUSESTATE mouseState;

	int width, height;
	int mouseX, mouseY;

	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

public:
	Input();
	Input(const Input& other);
	~Input();
};