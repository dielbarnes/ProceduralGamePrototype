//
// App.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <windowsx.h>
#include "GraphicsEngine.h"
#include "Timer.h"

LRESULT CALLBACK MainWindowProc(HWND hWindow,
								UINT uiMsg,
								WPARAM wParam, LPARAM lParam); // Additional message information

class App
{
public:
	App(HINSTANCE hInstance);
	~App();

	bool Initialize();
	void Run();

	LRESULT MessageHandler(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
	LPCTSTR m_applicationName = "App";
	HINSTANCE m_hInstance; // Application instance handle
	HWND m_hMainWindow; // Main window handle

	GraphicsEngine* m_pGraphicsEngine;
	Timer* m_pTimer;

	bool InitMainWindow(const int iScreenWidth, const int iScreenHeight);
	void CleanUpMainWindow();
};

static App* g_pApplicationHandle; // External pointer to this application
