//
// Engine.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <windowsx.h>
#include "Graphics.h"
#include "Timer.h"

LRESULT CALLBACK MainWindowProc(HWND hWindow,
								UINT uiMsg,
								WPARAM wParam, LPARAM lParam); // Additional message information

class Engine
{
public:
	Engine(HINSTANCE hInstance);
	~Engine();

	bool Initialize(LPCTSTR applicationName, int iWindowWidth,  int iWindowHeight);
	void Run();

	LRESULT MessageHandler(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
	LPCTSTR m_applicationName;
	HINSTANCE m_hInstance; // Application instance handle
	HWND m_hMainWindow; // Main window handle

	Graphics *m_pGraphics;
	Timer *m_pTimer;

	bool InitMainWindow(int iWindowWidth, int iWindowHeight);
	void CleanUpMainWindow();
};

static Engine *g_pApplicationHandle; // External pointer to this application
