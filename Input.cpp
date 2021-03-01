#include "Input.h"

Input::Input()
{
	directInput = 0;
	keyboard = 0;
	//mouse = 0;
	screenWidth = 0;
	screenHeight = 0;
}

Input::Input(const Input& other)
{

}

Input::~Input()
{

}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int width, int height)
{
	HRESULT hr;
	//mouseX = 0;
	//mouseY = 0;
	screenWidth = width;
	screenHeight = height;
	hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if(FAILED(hr))
	{
		std::cerr << "Failed to create DirectInput frame!" << std::endl;
		return false;
	}

	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if(FAILED(hr))
	{
		std::cerr << "Could not create device in frame setup!" << std::endl;
		return false;
	}

	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		std::cerr << "Could not set data format in frame setup!" << std::endl;
		return false;
	}
	
	
	hr = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(hr))
	{
		std::cerr << "Could not set cooperative level in frame setup!" << std::endl;
		return false;
	}


	hr = keyboard->Acquire();
	if(FAILED(hr))
	{
		std::cerr << "Could not get keyboard in frame setup!" << std::endl;
		return false;
	}
	/*
	hr = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if(FAILED(hr))
	{
		return false;
	}

	hr = mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr))
	{
		return false;
	}

	

	hr = mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))
	{
		return false;
	}
	
	hr = mouse->Acquire();
	if(FAILED(hr))
	{
		return false;
	}
	*/
	return true;
}


void Input::Shutdown()
{
	/*
	// Release the mouse.
	if(mouse)
	{
		mouse->Unacquire();
		mouse->Release();
		mouse = 0;
	}
	*/
	// Release the keyboard.
	if(keyboard)
	{
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}

	// Release the main interface to direct input.
	if(directInput)
	{
		directInput->Release();
		directInput = 0;
	}

	return;
}

bool Input::Frame()
{
	bool hr;


	// Read the current state of the keyboard.
	hr = ReadKeyboard();
	if(!hr)
	{
		return false;
	}
	/*
	// Read the current state of the mouse.
	hr = ReadMouse();
	if(!hr)
	{
		return false;
	}
	*/
	// Process the changes in the mouse and keyboard.
	//ProcessInput();

	return true;
}


bool Input::ReadKeyboard()
{
	HRESULT hr;


	// Read the keyboard device.
	hr = keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	if(FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			keyboard->Acquire();
		}
		else
		{
			std::cerr << "ERROR! Could not get keyboard device state!" << std::endl;
			return false;
		}
	}
		
	return true;
}

/*
bool Input::ReadMouse()
{
	HRESULT hr;


	// Read the mouse device.
	hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
	if(FAILED(hr))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((hr == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
} 

void Input::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	mouseX += mouseState.lX;
	mouseY += mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(mouseX < 0)  { mouseX = 0; }
	if(mouseY < 0)  { mouseY = 0; }
	
	if(mouseX > screenWidth)  { mouseX = screenWidth; }
	if(mouseY > screenHeight) { mouseY = screenHeight; }
	
	return;
}
*/
bool Input::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}
/*
void Input::GetMouseLocation(int& tempMouseX, int& tempMouseY)
{
	mouseX = tempMouseX;
	mouseY = tempMouseY;
	return;
}
*/

