#include "SystemHandler.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemHandler* systemHandler;
	bool result;
	
	
	// Create the system object.
	systemHandler = new SystemHandler;
	if(!systemHandler)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = systemHandler->Initialize();
	if(result)
	{
		systemHandler->Run();
	}

	// Shutdown and release the system object.
	systemHandler->Shutdown();
	delete systemHandler;
	systemHandler = 0;

	return 0;
}