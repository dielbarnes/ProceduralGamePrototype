//
// Main.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include <windows.h>
#include "App.h"
#include "Utils.h"

// Entry point
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   PSTR pCmdLine,				// Address of command line string for the application
				   int iCmdShow)				// Controls how the window is to be shown
{
	App* pApp = new App(hInstance);
	if (pApp->Initialize())
	{
		pApp->Run();
	}

	SAFE_DELETE(pApp);

	return 0;
}
