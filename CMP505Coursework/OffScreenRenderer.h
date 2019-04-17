//
// OffScreenRenderer.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#pragma once

#include <wincodec.h>
#include <d3d11.h>
#include <DirectXTK/ScreenGrab.h>
#include "Utils.h"

using namespace DirectX;

class OffScreenRenderer
{
public:
	OffScreenRenderer(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext);
	~OffScreenRenderer();

	ID3D11ShaderResourceView* GetOutputTexture();

	HRESULT Initialize(int iWindowWidth, int iWindowHeight);
	void SetRenderTarget();
	HRESULT SaveTextureToFile();

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11RenderTargetView *m_pRenderTargetView;
	ID3D11DepthStencilView *m_pDepthStencilView;
	ID3D11ShaderResourceView *m_pOutputTexture;
	ID3D11Texture2D *m_p2DTexture;
};
