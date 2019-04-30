//
// OffScreenRenderer.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#include "OffScreenRenderer.h"

#pragma region Init

OffScreenRenderer::OffScreenRenderer(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext)
{
	m_pDevice = pDevice;
	m_pImmediateContext = pImmediateContext;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_pOutputTexture = nullptr;
	m_p2DTexture = nullptr;
}

OffScreenRenderer::~OffScreenRenderer()
{
	SAFE_RELEASE(m_pRenderTargetView)
	SAFE_RELEASE(m_pDepthStencilView)
	SAFE_RELEASE(m_pOutputTexture)
	SAFE_RELEASE(m_p2DTexture)
}

HRESULT OffScreenRenderer::Initialize(int iWindowWidth, int iWindowHeight)
{
	HRESULT result = S_OK;

	// Create the 2D texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = iWindowWidth;
	textureDesc.Height = iWindowHeight;
	textureDesc.MipLevels = 1;														// Maximum number of mipmap levels in the texture; use 1 for a multisampled texture
	textureDesc.ArraySize = 1;														// Number of textures in the texture array
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;								// 32-bit unsigned-normalized-integer format that supports 8 bits per channel
	textureDesc.SampleDesc.Count = 1;												// Number of multisamples per pixel
	textureDesc.SampleDesc.Quality = 0;												// Image quality level
	textureDesc.Usage = D3D11_USAGE_DEFAULT;										// The texture is to be read from and written to by the GPU
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;	// The texture can be used both as a render target and as an input to a shader
	textureDesc.CPUAccessFlags = 0;													// CPU access is not required
	textureDesc.MiscFlags = 0;														// Other resource options
	result = m_pDevice->CreateTexture2D(&textureDesc,
										nullptr,		// Subresources for the texture
										&m_p2DTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create off-screen renderer 2D texture.", result);
		return result;
	}

	// Create the shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	result = m_pDevice->CreateShaderResourceView(m_p2DTexture, &shaderResourceViewDesc, &m_pOutputTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create off-screen renderer shader resource view.", result);
		return result;
	}

	// Create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	result = m_pDevice->CreateRenderTargetView(m_p2DTexture, &renderTargetViewDesc, &m_pRenderTargetView);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create off-screen renderer render target view.", result);
		return result;
	}
	
	// Create the depth stencil buffer

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.Width = iWindowWidth;
	depthStencilBufferDesc.Height = iWindowHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;					
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// Bind the texture as a depth-stencil target for the output-merger stage
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	ID3D11Texture2D *pDepthStencilBuffer = nullptr;
	result = m_pDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &pDepthStencilBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create off-screen renderer depth stencil buffer.", result);
		return result;
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depthStencilBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create off-screen renderer depth stencil view.", result);
		return result;
	}

	SAFE_RELEASE(pDepthStencilBuffer)

	return result;
}

#pragma endregion

#pragma region Getters

ID3D11ShaderResourceView* OffScreenRenderer::GetOutputTexture()
{
	return m_pOutputTexture;
}

#pragma endregion

#pragma region Render

void OffScreenRenderer::SetRenderTarget()
{
	// Bind the render target view and the depth stencil view to the output merger stage
	m_pImmediateContext->OMSetRenderTargets(1,						// Number of render targets to bind
											&m_pRenderTargetView,
											m_pDepthStencilView);

	// Clear the render target view
	float backgroundColor[4] = COLOR_F4(0.0f, 0.0f, 0.0f, 1.0f)
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, backgroundColor);

	// Clear the depth buffer to 1.0 (max depth)
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView,
											   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,	// Specifies the parts of the depth stencil to clear (clear both the depth buffer and the stencil)
											   1.0f,									// Clear the depth buffer with this value
											   0);										// Clear the stencil buffer with this value
}

HRESULT OffScreenRenderer::SaveTextureToFile()
{
	// Reference:
	// ScreenGrab (https://github.com/Microsoft/DirectXTK/wiki/ScreenGrab)

	return SaveWICTextureToFile(m_pImmediateContext, m_p2DTexture, GUID_ContainerFormatJpeg, L"texture.jpg");
}

#pragma endregion 
