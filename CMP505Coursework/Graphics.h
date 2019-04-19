//
// Graphics.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "Camera.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "Bloom.h"
#include "Utils.h"

// Link necessary libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

using namespace DirectX;

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Initialize(int iWindowWidth, int iWindowHeight, HWND hWindow);
	void OnMouseDown(int x, int y, HWND hWindow);
	void OnMouseUp();
	void OnMouseMove(WPARAM buttonState, int x, int y);
	bool Render(float fDeltaTime);

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	IDXGISwapChain *m_pSwapChain;
	ID3D11RenderTargetView *m_pRenderTargetView;
	ID3D11DepthStencilView *m_pDepthStencilView;
	ID3D11DepthStencilState *m_pDepthStencilStateDefault;
	ID3D11DepthStencilState *m_pDepthStencilStateLessEqual;
	ID3D11RasterizerState *m_pRasterizerStateDefault;
	ID3D11RasterizerState *m_pRasterizerStateNoCulling;
	ID3D11BlendState *m_pBlendStateInvSrcAlpha;
	ID3D11BlendState *m_pBlendStateNoPreBlendOp;
	ID3D11BlendState *m_pBlendStateDisabled;
	Camera *m_pCamera;
	POINT m_mousePosition;
	ResourceManager *m_pResourceManager;
	ShaderManager *m_pShaderManager;
	OffScreenRenderer *m_pOffScreenRenderer;
	PostProcessQuad *m_pPostProcessQuad;
	Bloom *m_pBloom;
	float fTotalTime;

	HRESULT InitDirect3D(int iWindowWidth, int iWindowHeight, HWND hWindow);
	void HandleKeyboardInput(float fDeltaTime);
	void SetRenderTarget();
	void UnbindPixelShaderResources();
};
