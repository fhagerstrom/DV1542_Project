#include "Input.h"

Input::Input()
{
	directInput = 0;
	keyboard = 0;
	mouse = 0;
}

Input::Input(const Input& other)
{
}

Input::~Input()
{
}

bool Input::ReadKeyboard()
{
	return true;
}

bool Input::ReadMouse()
{
	return true;
}

void Input::ProcessInput()
{

}