#include "ProjectEngine.h"
#include "WICTextureLoader.h" // DXTK

namespace dx = DirectX;

ProjectEngine::ProjectEngine(HINSTANCE hInstance) : DXInit(hInstance, "3D Project", 1024, 768, true)
{



#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateCmdWindow(500, 120, 32, 120);
	printf("Console window created successfully!");
#endif
}

ProjectEngine::~ProjectEngine()
{
	// Add resources to delete and release here

}


