#include <Windows.h>
#include <iostream>
#include "ProjectEngine.h"

int WINAPI wWinMain(_In_opt_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	ProjectEngine engine(hInstance);

	HRESULT hr = S_OK;

	hr = engine.InitWindow();
	if (FAILED(hr))
	{
		std::cerr << "Couldn't initialize window!" << std::endl;
		return hr;
	}

	hr = engine.InitDirectX();
	if (FAILED(hr))
	{
		std::cerr << "Couldn't initalize DirectX!" << std::endl;
		return hr;
	}


	return engine.Run();
	
}


