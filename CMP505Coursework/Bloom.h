//
// Bloom.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#pragma once

#include <DirectXTK/WICTextureLoader.h>
#include "Blur.h"

struct BloomExtractBuffer // For pixel shader
{
	float threshold;
	XMFLOAT3 padding;
};

struct BloomCombineBuffer // For pixel shader
{
	float bloomIntensity;
	float bloomSaturation;
	float sceneIntensity;
	float sceneSaturation;
	int shouldShowText;
	XMINT3 padding;
};

class Bloom
{
public:
	Bloom(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext);
	~Bloom();

	ID3D11ShaderResourceView* GetExtractTexture();
	ID3D11ShaderResourceView* GetBlurTexture();
	void SetShouldShowText(bool bShouldShowText);

	HRESULT Initialize(int iWindowWidth, int iWindowHeight);
	bool RenderBloomExtractToTexture(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pSceneTexture);
	bool RenderHorizontalBlurToTexture(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pInputTexture);
	bool RenderVerticalBlurToTexture(PostProcessQuad *pQuad);
	bool RenderBloomCombine(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pSceneTexture);

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pExtractPixelShader;
	ID3D11PixelShader *m_pCombinePixelShader;
	ID3D11InputLayout *m_pVertexInputLayout;
	ID3D11Buffer *m_pExtractBuffer;
	ID3D11Buffer *m_pCombineBuffer;
	ID3D11SamplerState *m_pSamplerState;
	OffScreenRenderer *m_pExtractRenderer;
	Blur *m_pBlur;
	ID3D11ShaderResourceView *m_pTextTexture;
	bool m_bShouldShowText;
};
