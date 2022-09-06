#include "Input.h"


Input::Input(HWND hwnd)
{
	this->hwnd = hwnd;
	directInput = 0;
	keyboard = 0;
	mouse = 0;

	for (int i = 0; i < 256; i++)
		keyboardState[i] = 0;
	ZeroMemory(&mouseState, sizeof(DIMOUSESTATE));
}

Input::~Input()
{
}

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

	/*
		1. Initialize direct input
		2. Create keyboard and mouse devices with directInput.
	*/

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if (FAILED(result)) {
		return false;
	}


	/*
		1. Set data format for keyboard and mouse, default formats.						(There are 5 different, 1 for keyboard, 2 for mouse, 2 for joystick)

		2. Establish the cooperative level for this instance of the device.
		The cooperative level determines how this instance of the device interacts with other instances of the device and the rest of the system.
		(Cooperative levels that they not gonna share with other programs)

		3. Acquire mouse and keyboard. (Basiclly makes them accecible to the input device)
	*/

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	result = mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	result = mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_BACKGROUND);
	if (FAILED(result)) 
	{
		return false;
	}

	result = mouse->Acquire();
	if (FAILED(result)) 
	{
		return false;
	}

	return true;
}

void Input::Shutdown()
{
	if (mouse) {
		mouse->Unacquire();
		mouse->Release();
		mouse = 0;
	}

	if (keyboard) {
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}

	if (directInput) {
		directInput->Release();
		directInput = 0;
	}
}

