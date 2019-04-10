//
// Main.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include <windows.h>
#include "Engine.h"
#include "Utils.h"

// Entry point
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   PSTR pCmdLine,				// Address of command line string for the application
				   int iCmdShow)				// Controls how the window is to be shown
{
	Engine *pEngine = new Engine(hInstance);
	if (pEngine->Initialize("Procedural Game Prototype", 1024, 768))
	{
		pEngine->Run();
	}

	SAFE_DELETE(pEngine);

	return 0;
}
