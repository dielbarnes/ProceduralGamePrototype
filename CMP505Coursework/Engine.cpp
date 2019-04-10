//
// Engine.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "Engine.h"

#pragma region Init

Engine::Engine(HINSTANCE hInstance)
{
	m_applicationName = "App";
	m_hInstance = hInstance;
	m_hMainWindow = nullptr;
	m_pGraphics = nullptr;
	m_pTimer = nullptr;
	g_pApplicationHandle = this;
}

Engine::~Engine()
{
	CleanUpMainWindow();

	SAFE_DELETE(m_pGraphics)
	SAFE_DELETE(m_pTimer)

	g_pApplicationHandle = nullptr;
}

bool Engine::Initialize(LPCTSTR applicationName, int iWindowWidth, int iWindowHeight)
{
	if (!InitMainWindow(iWindowWidth, iWindowHeight))
	{
		return false;
	}

	m_pGraphics = new Graphics();
	if (!m_pGraphics->Initialize(iWindowWidth, iWindowHeight, m_hMainWindow))
	{
		return false;
	}

	m_pTimer = new Timer();

	return true;
}

bool Engine::InitMainWindow(int iWindowWidth, int iWindowHeight)
{
	// Register the window class
	WNDCLASS windowClass;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;					// Redraw the entire window if a movement or size adjustment changes the width and height of the client area
	windowClass.lpfnWndProc = MainWindowProc;
	windowClass.cbClsExtra = 0;										// Number of extra bytes to allocate following the window class structure
	windowClass.cbWndExtra = 0;										// Number of extra bytes to allocate following the window instance
	windowClass.hInstance = m_hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // Class background brush handle
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = m_applicationName;
	RegisterClass(&windowClass);

	// Place the window at the center of the screen
	int iPositionX = (GetSystemMetrics(SM_CXSCREEN) - iWindowWidth) / 2;
	int iPositionY = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;

	// Create and show the window

	m_hMainWindow = CreateWindowEx(0,															// Extended window style
								   m_applicationName,											// Window class
								   m_applicationName,											// Window text
								   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,	// Window style
								   iPositionX,
								   iPositionY,
								   iWindowWidth,
								   iWindowHeight,
								   NULL,														// Parent window    
								   NULL,														// Menu
								   m_hInstance,													// Application instance handle
								   NULL);														// Additional application data
	if (m_hMainWindow == NULL)
	{
		MessageBox(0, "Failed to create window.", "", 0);
		return false;
	}

	ShowWindow(m_hMainWindow,
			   SW_SHOW);		// Activates the window and displays it in its current size and position

	return true;
}

#pragma endregion

#pragma region Message Handling

LRESULT CALLBACK MainWindowProc(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
	default:
		// Pass all other messages to the Engine message handler
		return g_pApplicationHandle->MessageHandler(hWindow, uiMsg, wParam, lParam);
	}
}

LRESULT Engine::MessageHandler(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
	case WM_LBUTTONDOWN:
		m_pGraphics->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hWindow);
		break;
	case WM_LBUTTONUP:
		m_pGraphics->OnMouseUp();
		break;
	case WM_MOUSEMOVE:
		m_pGraphics->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	}

	return DefWindowProc(hWindow, uiMsg, wParam, lParam);
}

#pragma endregion

#pragma region Run

void Engine::Run()
{
	// Run the main loop until there is a quit message from the window
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// Handle window events
		while (PeekMessage(&msg,
						   NULL,		// Retrieve messages for any window that belongs to the current thread, and any message on the current thread's message queue whose hwnd value is NULL
						   0, 0,		// If wMsgFilterMin and wMsgFilterMax are both 0, all available messages are returned (no range filtering is performed)
						   PM_REMOVE))	// Messages are removed from the queue after processing
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Render
		m_pTimer->Update();
		if (!m_pGraphics->Render(m_pTimer->GetDeltaTime()))
		{
			break;
		}
	}
}

#pragma endregion

#pragma region CleanUp

void Engine::CleanUpMainWindow()
{
	DestroyWindow(m_hMainWindow);
	m_hMainWindow = nullptr;

	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;
}

#pragma endregion
