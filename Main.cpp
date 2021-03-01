#include "WindowSetup.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	WindowSetup* window;
	bool result;

	// Create window object
	window = new WindowSetup;
	if (!window)
	{
		std::cerr << "Window object not created!" << std::endl;
		return -1;
	}

	// Initalize and run
	result = window->Initialize();
	if (result)
	{
		window->Run();
	}

	// Release objects
	window->ReleaseObjects();
	delete window;

	return 0;
}


